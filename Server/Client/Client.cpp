#include <time.h>
#include <algorithm>
#include <cstdlib>

#include "Client.hpp"
#include "FileTools.hpp"
#include "AppData.hpp"
#include "Tools.hpp"

Client::Client(tcp::io_service& io_service) : _socket(io_service), _timer(io_service) {
	this->_timestamp = 0.0;
	this->_protocol = new LProtocol(this);
	Message *message = headerMessage(VSP::LOGIN);
	this->_userInfo = NULL;
	this->_fileBuffer = "";
	/*
	_timer.expires_from_now(boost::posix_time::seconds(1));
	_timer.async_wait(boost::bind(&Client::messageHandler, this));
	*/

	std::cout << "_appDirectory: " << AppData::getInstance()._appDirectory << std::endl;;
}
Client::~Client() {
	boost::system::error_code err;

	_timer.cancel();
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
	if (checkError(error))
		return;
	std::cout << "Client::readHeaderHandler: " << "number bit available: "<< this->_socket.available() << std::endl;

	this->_inputMessage->decodeHeader();
	this->_inputMessage->debug();

	//TODO: if bad header -> 	readheader(error); and clean the Message
	if (!this->_inputMessage->isValidHeader())
		std::cout << "-------------Client::readHeaderHandler -> invalid Message-------------" << std::endl;
	if (bytes_transferred != this->_inputMessage->getHeaderLength())
		std::cout << "-------------Client::readHeaderHandler -> size Message [" << this->_inputMessage->getBodyLength() << "] != bytes_transferred ["  << bytes_transferred << "]-------------" << std::endl;
	readBody(error);
	//readHeader(error);
}

void Client::readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	if (checkError(error))
		return;

	std::cout << "Client::readBodyHandler: " << "number bit available: "<< this->_socket.available() << std::endl;

	if (bytes_transferred != this->_inputMessage->getBodyLength())
		std::cout << "-------------Client::readHeaderHandler -> size Message [" << this->_inputMessage->getBodyLength() << "] != bytes_transferred ["  << bytes_transferred << "]-------------" << std::endl;


	this->_protocol->receiveMessage(*this->_inputMessage);
	//this->_messages.push_back(this->_inputMessage);

	readHeader(error);
}

void Client::writeFinish(const boost::system::error_code &error, uint8_t *data) {
	//TODO: clean message
	if (data != NULL)
		delete[] data;

	if (checkError(error))
		return;
	std::cout << "Client::writeFinish. SUCESS !" << std::endl;
}

/* SEND / RECEIVE MESSAGE */

void Client::messageHandler() {
	/* CHECK EACH CLIENT */

	while (!this->_messages.empty())
	{	
		this->_protocol->receiveMessage(*this->_messages.front());
		this->_messages.pop_front();
	}

	_timer.expires_from_now(boost::posix_time::seconds(5));
	_timer.async_wait(boost::bind(&Client::messageHandler, this));

}


void Client::sendMessage(Message &message) {
	boost::asio::async_write(this->_socket,
		boost::asio::buffer((unsigned char *)message.getData(), message.getDataLength()),
		boost::bind(&Client::writeFinish, this,
		boost::asio::placeholders::error, (unsigned char *)message.getData()));
}

/* CALLBACK FOR PROTOCOL */

//TODO: do the job and answer to the client

void Client::loginHandler(MessageLogin &message) {
	char result;

	Message *answerMessage = this->headerMessage(VSP::LOGIN_RESULT);
	//TODO: check if user and password in DB are OK and
	//		if client is not already connected
	result = VSP::OK;

	if (result == VSP::OK) {
		Message *messageLogin = headerMessage(VSP::LOGIN);
		_userInfo = new MessageLogin(*messageLogin, message._login.login, message._login.password);
	}
	MessageLoginResult *customMessage = new MessageLoginResult(*answerMessage, VSP::LOGIN_RESULT, result);
	customMessage->encodeBody();
	customMessage->encodeData();

	//sendMessage(*customMessage);

	message.clean();
}

void Client::loginResultHandler(MessageLoginResult &message) {
	//INFO: never call
}

void Client::logOutHandler(MessageLogOut &message) {
	//INFO: client want to be disconnected
	disconect();
}

void Client::plateHandler(MessagePlate &message) {
	//INFO: client want to know to the plate number for code_file
	//if can answer directly send MessagePlate with the good value
	//else put code_file for this client to the treatment queue

}

void Client::fileHandler(MessageFile &message) {
	std::string b(message._file.file, message._file.to_read);

	this->_fileBuffers.push_back(b);
	if (message._file.indx == message._file.max_indx) {
		std::string imageBuffer;

		while (!_fileBuffers.empty())
		{
			imageBuffer += _fileBuffers.front();
			_fileBuffers.pop_front();
		}

		bool ret = FileTools::writeStringToFile(imageBuffer, "serverVerifUpload.jpg");
		if (!ret) {
			std::cout << "FAILED: " << "Writting file on path: " << message._file.code_file << std::endl;
		}
	}
	message.clean();
	/*this->_fileBuffer += message._file.file;
	if (message._file.indx == message._file.max_indx) {
		std::string filePath = "";//AppData::getInstance()._appDirectory;
		filePath  += message._file.code_file;
		std::cout << "Writting file on path: " << filePath << std::endl;
		bool ret = FileTools::writeStringToFile(this->_fileBuffer, filePath);
		if (!ret) {
			std::cout << "FAILED: " << "Writting file on path: " << filePath << std::endl;
		}
		this->_fileBuffer = "";
	}*/
}

void Client::unknowMessageHandler(Message &message) {

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
		std::cout << " login: " << _userInfo->_login.login << " password: " << _userInfo->_login.password;
	}
	std::cout << std::endl;
}

void Client::debug() {

}