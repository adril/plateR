#include <time.h>
#include <algorithm>
#include <cstdlib>

#include "Client.hpp"
#include "FileTools.hpp"
#include "AppData.hpp"
#include "Tools.hpp"

Client::Client(tcp::io_service& io_service) : _socket(io_service), _getUpdatedTimer(io_service) {
	this->_timestamp = 0.0;
	this->_protocol = new LProtocol(this);
	//Message *message = headerMessage(VSP::LOGIN);
	this->_userInfo = NULL;
	this->_fileBuffer = "";
	this->_isLogin = false;

	/* Timer to get Updated */
	this->_getUpdatedTimer.expires_from_now(boost::posix_time::seconds(1));
	this->_getUpdatedTimer.async_wait(boost::bind(&Client::getUpdatedHandler, this, _1));

	this->_sendFileList.push_back(FileAnnotation("004.jpg", "004.jpg"));
	this->_sendFileList.push_back(FileAnnotation("005.jpg", "005.jpg"));

	std::cout << "appDirectory: " << AppData::getInstance()._appDirectory << std::endl;;
}
Client::~Client() {
	boost::system::error_code err;

	this->_getUpdatedTimer.cancel();
	this->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	this->_socket.close(err);
}

void Client::start() {
	const boost::system::error_code error;

	readHeader(error);
}

void Client::readHeader(const boost::system::error_code& error) {
	if (checkError(error))
		return;

	this->_inputMessage = new Message();

	this->_socket.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getHeader(), this->_inputMessage->getHeaderLength()),
		boost::bind(&Client::readHeaderHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void Client::readBody(const boost::system::error_code& error) {
	if (checkError(error))
		return;
	this->_socket.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getBody(), this->_inputMessage->getBodyLength()),
		boost::bind(&Client::readBodyHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void Client::readHeaderHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	//TODO: do job on the message
	if (checkError(error)) {
		return;
	}
	std::cout << "Client::readHeaderHandler: " << "number bit available: "<< this->_socket.available() << std::endl;

	this->_inputMessage->decodeHeader();
	this->_inputMessage->debug();

	//TODO: if bad header -> 	readheader(error); and clean the Message
	if (!this->_inputMessage->isValidHeader()){
		std::cout << "-------------Client::readHeaderHandler -> invalid Message-------------" << std::endl;

		disconect();
		//readHeader(error);
		return;
	}
	if (bytes_transferred != this->_inputMessage->getHeaderLength()) {
		std::cout << "-------------Client::readHeaderHandler -> size Message [" << this->_inputMessage->getBodyLength() << "] != bytes_transferred ["  << bytes_transferred << "]-------------" << std::endl;
		disconect();
		return;

	}
	readBody(error);
	//readHeader(error);
}

void Client::readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	if (checkError(error)) {
		return;
	}
	std::cout << "Client::readBodyHandler: " << "number bit available: "<< this->_socket.available() << std::endl;

	if (bytes_transferred != this->_inputMessage->getBodyLength())
		std::cout << "-------------Client::readHeaderHandler -> size Message [" << this->_inputMessage->getBodyLength() << "] != bytes_transferred ["  << bytes_transferred << "]-------------" << std::endl;


	if (this->_protocol->receiveMessage(*this->_inputMessage))//INFO: if false stop reading
		readHeader(error);
}

void Client::writeFinish(const boost::system::error_code &error, uint8_t *data) {
	//TODO: clean message
	if (data != NULL)
		delete[] data;

	//displayUserInfo();
	if (checkError(error)) {
		std::cout << "Meassage send: SUCESS !" << std::endl;
		return;
	}
	std::cout << "Meassage send: SUCESS !" << std::endl;
}

/* SEND / RECEIVE MESSAGE */

void Client::sendMessage(Message &message) {
	boost::asio::async_write(this->_socket,
		boost::asio::buffer((unsigned char *)message.getData(), message.getDataLength()),
		boost::bind(&Client::writeFinish, this,
		boost::asio::placeholders::error, (unsigned char *)message.getData()));
}

void Client::sendRecordedPlate() {
	//TODO: -get plate list from db
	//		-create a message for it and send it [OK]
	size_t coutRecordedPlate = 42;
	std::string codePlate = "plate004";//INFO: plate value
	std::string name = "Plate recorded test";
	std::string codeFile = "004.jpg";//INFO: foreign key between a plate and a file

	for (size_t i = 0; i <= coutRecordedPlate; ++i) {
		Message *answerMessage = this->headerMessage(VSP::PLATE);
		answerMessage->decodeHeader();
		MessagePlate *customMessage = new MessagePlate(*answerMessage, 'a', (char*)codePlate.c_str(), (char*)name.c_str(), (char*)codeFile.c_str(), VSP::RECORDED);
		customMessage->encodeBody();
		customMessage->encodeData();
		customMessage->debug();

		sendMessage(*customMessage);
		//TODO: send the file
	}
}

void Client::sendInfoPlate(std::string codeFile, char state) {
	if (!this->_delegate)
		return;
		std::string filePathSmallPlate = this->_delegate->recognizePlate(this, codeFile);
		std::string plateValue = this->_delegate->getPlateValue(this, codeFile);
		std::string codePlate = codeFile + "-small";

		Message *answerMessage = this->headerMessage(VSP::PLATE);
		answerMessage->decodeHeader();
		MessagePlate *customMessage = new MessagePlate(*answerMessage, 'a', (char*)codePlate.c_str(),
				(char*)plateValue.c_str(), (char*)codeFile.c_str(), state);
		customMessage->encodeBody();
		customMessage->encodeData();
		customMessage->debug();

		sendMessage(*customMessage);

}

/* GET UPDATED */

void Client::getUpdatedHandler(const boost::system::error_code &error) {
	//WARNING: may have to use mutex

	if (error == boost::asio::error::operation_aborted)
		return;

	if (this->_isLogin == true) {

		//INFO: Plate to treat
		std::cout << "getUpdatedHandler:" << _plateToTreatList.size() << std::endl;
		while (!this->_plateToTreatList.empty()) {//ERROR: bad while -> ne have to pop if the file is not on the server
			std::cout << "_this->_plateToTreatList.front()._codeFile: " << this->_plateToTreatList.front()._codeFile << std::endl;
			sendInfoPlate(this->_plateToTreatList.front()._codeFile, this->_plateToTreatList.front()._state);
			this->_sendFileList.pop_front();
		}


		//INFO: File
		std::cout << "_sendFileList: " << this->_sendFileList.size() << std::endl;
		while (!this->_sendFileList.empty()) {
			std::cout << "_fileAnnotationList.front()._filePath: " << this->_sendFileList.front()._filePath << std::endl;
			std::cout << "_fileAnnotationList.front()._codeFile: " << this->_sendFileList.front()._codeFile << std::endl;
			sendFile(this->_sendFileList.front()._filePath);
			this->_sendFileList.pop_front();
		}

		//INFO: Message
		while (!this->_sendMessageList.empty()) {
			sendMessage(*this->_sendMessageList.front());
			this->_sendMessageList.pop_front();
		}
	}

	_getUpdatedTimer.expires_from_now(boost::posix_time::seconds(5));
	_getUpdatedTimer.async_wait(boost::bind(&Client::getUpdatedHandler, this, _1));

}

/* CALLBACK FOR IPROTOCOL */

bool Client::loginHandler(MessageLogin &message) {
	char result;

	Message *answerMessage = this->headerMessage(VSP::LOGIN_RESULT);
	//TODO: check if user and password in DB are OK and
	//		if client is not already connected
	result = VSP::OK;

	MessageLoginResult *customMessage = new MessageLoginResult(*answerMessage, VSP::LOGIN_RESULT, result);
	customMessage->encodeBody();
	customMessage->encodeData();

	sendMessage(*customMessage);

	if (result == VSP::OK) {
		this->_isLogin = true;
		Message *messageLogin = headerMessage(VSP::LOGIN);
		_userInfo = new MessageLogin(*messageLogin, message._login.login, message._login.password);
		//TODO: send recorded plate [OK] - DB
		sendRecordedPlate();
	}

	message.clean();
	return true;
}

bool Client::loginResultHandler(MessageLoginResult &message) {
	//INFO: never call [OK]
	message.clean();
	return true;
}

bool Client::logOutHandler(MessageLogOut &message) {
	//INFO: client want to be disconnected [OK]
	disconect();
	message.clean();
	return true;
}

bool Client::plateHandler(MessagePlate &message) {
	//INFO: client want to know to the plate number for code_file
	//if can answer directly send MessagePlate with the good value
	//else put code_file for this client to the treatment queue

	if (!this->_delegate)
		return true;
	if (message._plate.state == VSP::LIVE) {
		//INFO: file to treat is not on server
		this->_plateToTreatList.push_back(PlateAnnotation(message._plate.code_file, VSP::LIVE));
	}
	else if (message._plate.state == VSP::RECORDED) {//INFO: mean image to treat already on the server
		sendInfoPlate(message._plate.code_file, VSP::RECORDED);

	}

	message.clean();
	return true;
}

bool Client::fileHandler(MessageFile &message) {
	std::string b(message._file.file, message._file.to_read);

	this->_fileBuffer += b;
	if (message._file.indx == message._file.max_indx) {
		std::string filePath = "";//AppData::getInstance()._appDirectory;
		filePath  += message._file.code_file;
		std::cout << "Writting file on path: " << filePath << std::endl;
		bool ret = FileTools::writeStringToFile(this->_fileBuffer, filePath);
		if (!ret) {
			std::cout << "FAILED: " << "Writting file on path: " << filePath << std::endl;
		}
		else {
			std::cout << "SUCCESS: " << "Writting file on path: " << filePath << std::endl;
		}
		this->_fileBuffer = "";
	}
	message.clean();
	return true;
}

bool Client::unknowMessageHandler(Message &message) {
	if (message.getError() != VSP::ERROR_NOTHING) {
		disconect();
		message.clean();
		return false;
	}
	return true;
}

/* Send File */

void Client::sendFile(std::string filename) {//INFO: may be add code file as param
	std::string buffer = FileTools::readStringFromFile(filename.c_str());

	if (buffer.length() == 0)
		return;

	Tools::Range rangeToRead;

	size_t dataFileLength = buffer.length();
	size_t nbMessage = (dataFileLength / VSP::FILE_SIZE < 1) ? 1 : dataFileLength / VSP::FILE_SIZE;
	size_t dataWritten = 0;
	char dataToSend[VSP::FILE_SIZE];

	for (unsigned int i = 0; i <= nbMessage; i++) {
		rangeToRead.location = dataWritten;
		rangeToRead.length = (dataWritten + VSP::FILE_SIZE > dataFileLength) ? dataFileLength - dataWritten : VSP::FILE_SIZE;
		std::memset(dataToSend, '\0', VSP::FILE_SIZE);
		std::memcpy(dataToSend, buffer.c_str() + rangeToRead.location, rangeToRead.length);
		Message *message = headerMessage(VSP::FILE);

		MessageFile *customMessage = new MessageFile(*message, 'a', i, nbMessage, (char*)filename.c_str(), rangeToRead.length, dataToSend);
		customMessage->encodeBody();
		customMessage->encodeData();
		sendMessage(*customMessage);

		std::string data(dataToSend, rangeToRead.length);
		dataWritten += rangeToRead.length;
	}
	std::cout << "FileTools::file send" << std::endl;
}


/* Message */

Message *Client::headerMessage(char bodyType) {
	Message *message = new Message();
	message->encodeHeader(bodyType);
	message->decodeHeader();//INFO: alloc body
	return message;
}

/* ERROR */

void Client::disconect() {
	std::cout << std::endl << "Client disconnected" << std::endl;
	this->_isLogin = false;
	displayUserInfo();
	if (this->_delegate)
		this->_delegate->disconectClient(this);
	delete this;
}

bool Client::checkError(const boost::system::error_code& error) {
	if (error) {
		//TODO: check the error value
		disconect();
		return true;
	}
	return false;
}

/* GETTER */

tcp::socket& Client::getSocket() {
	return _socket;
}

std::string	Client::getIpAsString() const {
	return _socket.remote_endpoint().address().to_string();
}

/* SETTER */

void Client::setDelegate(IServerDelegate *delegate) {
	this->_delegate = delegate;
}

/* DEBUG */

void Client::displayUserInfo() const {
	std::cout << "[" << getIpAsString() << "]";
	if (this->_userInfo) {
		std::cout << " login: " << _userInfo->_login.login; /*<< " password: " << _userInfo->_login.password;*/
	}
	std::cout << std::endl;
}

void Client::debug() {

}