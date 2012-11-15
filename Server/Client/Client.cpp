#include <time.h>
#include <algorithm>
#include <cstdlib>

#include "Client.hpp"
#include "FileTools.hpp"
#include "AppData.hpp"
#include "Tools.hpp"
#include "DB.hpp"
#include "StringTools.hpp"

Client::Client(tcp::io_service& io_service) : _socket(io_service), _getUpdatedTimer(io_service) {
	this->_timestamp = 0.0;
	this->_protocol = new LProtocol(this);
	this->_userInfo = NULL;
	this->_fileBuffer = "";
	this->_isLogin = false;
	this->_getUpdatedInterval = 20;

	/* Timer to get Updated */
	this->_getUpdatedTimer.expires_from_now(boost::posix_time::seconds(this->_getUpdatedInterval));
	this->_getUpdatedTimer.async_wait(boost::bind(&Client::getUpdatedHandler, this, _1));

	std::cout << ".getFileDirectoryPath: " << AppData::getInstance().getFileDirectoryPath() << std::endl;;
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
	if (checkError(error))
		return;

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
	//TODO: -get plate list from db [OK]
	//		-create a message for it and send it [OK]
	std::list<DBDATA::Plate> recordedPlate = DB::get_all_recorded_plate(this->_userInfo->_login.login);
	size_t coutRecordedPlate = recordedPlate.size();

	for (size_t i = 0; i < coutRecordedPlate; ++i) {
		DBDATA::Plate plate = recordedPlate.front();

		Message *answerMessage = this->headerMessage(VSP::PLATE);
		answerMessage->decodeHeader();
		MessagePlate *customMessage = new MessagePlate(*answerMessage, 'a', (char*)plate.codePlate.c_str(), (char*)plate.name.c_str(), (char*)plate.codeFile.c_str(), VSP::RECORDED);
		customMessage->encodeBody();
		customMessage->encodeData();
		customMessage->debug();

		std::string basePath = (plate.path);
		sendMessage(*customMessage);
		sendFile(basePath + plate.name, plate.codeFile);//WARNING
		recordedPlate.pop_front();
	}
}

void Client::sendInfoPlate(std::string codeFile, char state) {
	if (!this->_delegate)
		return;
	std::string filePathSmallPlate = this->_delegate->recognizePlate(this, codeFile);
	std::string plateValue = this->_delegate->getPlateValue(this, codeFile);
	std::string codePlate = codeFile + "-small";//WARNING

	Message *answerMessage = this->headerMessage(VSP::PLATE);
	answerMessage->decodeHeader();
	MessagePlate *customMessage = new MessagePlate(*answerMessage, 'a', (char*)codePlate.c_str(),
				(char*)plateValue.c_str(), (char*)codeFile.c_str(), state);//WARNING
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

		_fileToTreatList = DB::get_file_to_treat(this->_userInfo->_login.login);
		// look the BDD for untreated picture (image reco)
		std::cout << "getUpdatedHandler: fileTreatList : " << _fileToTreatList.size() << std::endl;
		while (!this->_fileToTreatList.empty()) {
			DBDATA::File file = this->_fileToTreatList.front();
			std::string codePlate("codePlate");

			file.state = 1;//WARNING
			DB::update_file(file);
			//ret = reco.treatement(file.path + file.name, file.path + file.name + "-plate");

			std::string buff = FileTools::readStringFromFile(file.path + file.name);
			FileTools::writeStringToFile(buff, file.path + "plate-" + file.name);

			if (codePlate != "") {
				file.state = 2;
				DB::add_plate(DBDATA::Plate(codePlate,  "plate-" + file.name, file.codeFile, file.path, this->_userInfo->_login.login, 1));
				DB::update_file(file);
				sendFile(file.path + "plate-" + file.name, file.codeFile);
				sendInfoPlate(codePlate, VSP::LIVE);
				/*
				Message *answerMessage = this->headerMessage(VSP::PLATE);
				answerMessage->decodeHeader();
				MessagePlate *customMessage = new MessagePlate(*answerMessage, 'a', (char*)codePlate.c_str(),
					(char*)file.name.c_str(), (char*)file.name.c_str(), VSP::RECORDED);//WARNING
				customMessage->encodeBody();
				customMessage->encodeData();
				customMessage->debug();
				sendMessage(*customMessage);
				*/
			}
			else {
				file.state = 0;
				DB::update_file(file);
			}
			this->_fileToTreatList.pop_front();
		}
		// look the BDD for untreated plate  (not send to the client)

		//INFO: Plate to treat
		std::cout << "getUpdatedHandler:" << _plateToTreatList.size() << std::endl;
		while (!this->_plateToTreatList.empty()) {//ERROR: bad while -> ne have to pop if the file is not on the server
			std::cout << "_this->_plateToTreatList.front()._codeFile: " << this->_plateToTreatList.front()._codeFile << std::endl;
			std::cout << "_this->_plateToTreatList.front()._state: " << ((this->_plateToTreatList.front()._state == VSP::RECORDED) ? "VSP::RECORDED" : "VSP::LIVE") << std::endl;


			sendInfoPlate(this->_plateToTreatList.front()._codeFile, this->_plateToTreatList.front()._state);
			this->_sendFileList.pop_front();
		}


		//INFO: File
		std::cout << "_sendFileList: " << this->_sendFileList.size() << std::endl;
		while (!this->_sendFileList.empty()) {
			std::cout << "_fileAnnotationList.front()._filePath: " << this->_sendFileList.front()._filePath << std::endl;
			std::cout << "_fileAnnotationList.front()._codeFile: " << this->_sendFileList.front()._codeFile << std::endl;
			sendFile(this->_sendFileList.front()._filePath, this->_sendFileList.front()._codeFile);
			this->_sendFileList.pop_front();
		}

		//INFO: Message
		while (!this->_sendMessageList.empty()) {
			sendMessage(*this->_sendMessageList.front());
			this->_sendMessageList.pop_front();
		}
	}

	_getUpdatedTimer.expires_from_now(boost::posix_time::seconds(this->_getUpdatedInterval));
	_getUpdatedTimer.async_wait(boost::bind(&Client::getUpdatedHandler, this, _1));

}

/* CALLBACK FOR IPROTOCOL */

bool Client::loginHandler(MessageLogin &message) {
	char result;

	Message *answerMessage = this->headerMessage(VSP::LOGIN_RESULT);
	DBDATA::User	*currentUser = DB::get_user_by_login(message._login.login);

	result = VSP::KO;
	if (currentUser && currentUser->authUser(message._login.password))
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
		sendRecordedPlate();//WARNING
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
		std::string filePath = AppData::getInstance().getFileDirectoryPath();
		std::cout << "Writting file on path: " << filePath + message._file.code_file << std::endl;
		bool ret = FileTools::writeStringToFile(this->_fileBuffer, filePath + message._file.code_file);
		if (!ret) {
			std::cout << "FAILED: " << "Writting file on path: " << filePath  << message._file.code_file << std::endl;
		}
		else {
			// ADD File information on the database for treatement
			DB::add_file(DBDATA::File(message._file.code_file, filePath, message._file.code_file, this->_userInfo->_login.login));//File(message._file.code_file, filePath, this->_userInfo->_login.login, 0)
			std::cout << "SUCCESS: " << "Writting file on path: " << filePath << message._file.code_file << std::endl;
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

void Client::sendFile(std::string filename, std::string codeFile) {//INFO: may be add code file as param
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

		MessageFile *customMessage = new MessageFile(*message, 'a', i, nbMessage, (char*)codeFile.c_str(), rangeToRead.length, dataToSend);
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
	return  _socket.remote_endpoint().address().to_string();
}

/* SETTER */

void Client::setDelegate(IServerDelegate *delegate) {
	this->_delegate = delegate;
}

/* DEBUG */

void Client::displayUserInfo() const {
	std::cout << "[" << getIpAsString() << "]";//ERROR: if socket close -> throwing exeption
	if (this->_userInfo) {
		std::cout << " login: " << _userInfo->_login.login; /*<< " password: " << _userInfo->_login.password;*/
	}
	std::cout << std::endl;
}

void Client::debug() {

}