#include "Client.hpp"

Client::Client(boost::asio::io_service& io_service)
	: _stopped(false), _socket(io_service), _deadline(io_service), _heartbeat_timer(io_service), _getUpdatedTimer(io_service) {
		this->_protocol = new LProtocol(this);
		this->_delegate = NULL;
		this->_fileBuffer = "";
		this->_isLogin = false;
}

Client::~Client() {
	boost::system::error_code err;
}

void Client::start(tcp::resolver::iterator endpoint_iter) {
	startConnect(endpoint_iter);
	this->_deadline.async_wait(boost::bind(&Client::check_deadline, this));
}

void Client::disconnect() {
	this->_stopped = true;
	boost::system::error_code ignored_ec;
	
	this->_getUpdatedTimer.cancel();
	this->_socket.close(ignored_ec);
	this->_deadline.cancel();
	this->_heartbeat_timer.cancel();
}

void Client::startConnect(tcp::resolver::iterator endpoint_iter) {
	if (endpoint_iter != tcp::resolver::iterator()) {
		std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";
		this->_deadline.expires_from_now(boost::posix_time::seconds(60));
		this->_socket.async_connect(endpoint_iter->endpoint(),
			boost::bind(&Client::handleConnect,
			this, _1, endpoint_iter));
	}
	else //INFO: may be notify the delegate
		disconnect();
}

void Client::handleConnect(const boost::system::error_code& ec, tcp::resolver::iterator endpoint_iter) {
	if (this->_stopped)
		return;

	if (!this->_socket.is_open()) {
		std::cout << "Connect timed out\n";
		startConnect(++endpoint_iter);
	}
	else if (ec) {
		std::cout << "Connect error: " << ec.message() << "\n";
		this->_socket.close();
		startConnect(++endpoint_iter);
	}
	else {
		std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";
		//INFO: start timer
		/* Timer to get Updated */
		this->_getUpdatedTimer.expires_from_now(boost::posix_time::seconds(1));
		this->_getUpdatedTimer.async_wait(boost::bind(&Client::getUpdatedHandler, this, _1));


		sendLogin("jullien", "1234567");
		startRead();
		//sendFile("Smile-3.jpg", "Smile-3.jpg");
		//sendFile("001.jpg", "001.jpg");
		//sendFile("002.jpg", "002.jpg");
		//sendFile("003.jpg", "003.jpg");
		//sendFile("001.jpg", "001.jpg");
		//sendFile("002.jpg", "002.jpg");
		//sendFile("003.jpg", "003.jpg");
	}
}

void Client::getUpdatedHandler(const boost::system::error_code &error) {
	//WARNING: may have to use mutex
	//INFO: info may be do it with Messages

	if (error == boost::asio::error::operation_aborted)
		return;

	if (this->_isLogin == true) {

		//TODO: check if new image have been add

		//INFO: File
		std::cout << "_sendFileList: " << this->_sendFileList.size() << std::endl;
		while (!this->_sendFileList.empty()) {
			std::cout << "_fileAnnotationList.front()._filePath: " << this->_sendFileList.front()._filePath << std::endl;
			std::cout << "_fileAnnotationList.front()._codeFile: " << this->_sendFileList.front()._codeFile << std::endl;
			sendFile(this->_sendFileList.front()._filePath, this->_sendFileList.front()._codeFile);
			this->_sendFileList.pop_front();
		}

		//INFO: debug test
		//this->_sendFileList.push_back(FileAnnotation("001.jpg", "001.jpg"));
		//this->_sendFileList.push_back(FileAnnotation("002.jpg", "002.jpg"));
	}

	_getUpdatedTimer.expires_from_now(boost::posix_time::seconds(10));
	_getUpdatedTimer.async_wait(boost::bind(&Client::getUpdatedHandler, this, _1));
}


void Client::startRead() {
	const boost::system::error_code error;
	read(error);
}

void Client::read(const boost::system::error_code& error) {
	if (error) {
		std::cout << "Client::read. ERROR !" << std::endl;
		this->_socket.close();
	}
	else
		readHeader(error);
}

void Client::readHeader(const boost::system::error_code& error) {
	this->_inputMessage = new Message();
	this->_socket.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getHeader(), this->_inputMessage->getHeaderLength()),
		boost::bind(&Client::readHeaderHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void Client::readBody(const boost::system::error_code& error) {
	this->_socket.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getBody(), this->_inputMessage->getBodyLength()),
		boost::bind(&Client::readBodyHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void Client::readHeaderHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	this->_inputMessage->decodeHeader();

	if (!this->_inputMessage->isValidHeader()){
		std::cout << "-------------Client::readHeaderHandler -> invalid Message-------------" << std::endl;

		disconnect();
		return;
	}
	if (bytes_transferred != this->_inputMessage->getHeaderLength()) {
		std::cout << "-------------Client::readHeaderHandler -> size Message [" << this->_inputMessage->getBodyLength() << "] != bytes_transferred ["  << bytes_transferred << "]-------------" << std::endl;
		disconnect();
		return;

	}
	readBody(error);
}

void Client::readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	std::cout << "{BODY} bytes_transferred : " << bytes_transferred << std::endl;
	this->_protocol->receiveMessage(*this->_inputMessage);

	read(error);
}

/* Send */

void Client::sendMessage(Message &msg) {
	boost::asio::async_write(this->_socket, 
		boost::asio::buffer((unsigned char *)msg.getData(), msg.getDataLength()),
		boost::bind(&Client::handle_write, this, _1));
}

void Client::sendMessageQueue(std::queue<Message*> msgQueue) {
	while (!msgQueue.empty())
	{
		sendMessage(*msgQueue.front());
		msgQueue.pop();
	}
}

void Client::sendLogin(std::string username, std::string password) {
	Message *message = headerMessage(VSP::LOGIN);

	MessageLogin *customMessage = new MessageLogin(*message,  (char*)username.c_str(), (char*)password.c_str());
	customMessage->encodeBody();
	customMessage->encodeData();
	sendMessage(*customMessage);
}

void Client::sendLogout() {
	Message *message = headerMessage(VSP::LOGOUT);

	MessageLogOut *customMessage = new MessageLogOut(*message);
	customMessage->encodeBody();
	customMessage->encodeData();
	sendMessage(*customMessage);
}

void Client::sendRecognizePlate(std::string filePath, std::string codeFile) {
	std::string codePlate = "";//INFO: plate value
	std::string name = "";

	Message *answerMessage = this->headerMessage(VSP::PLATE);
	answerMessage->decodeHeader();
	MessagePlate *customMessage = new MessagePlate(*answerMessage, 'a', (char*)codePlate.c_str(), (char*)name.c_str(), (char*)codeFile.c_str(), VSP::RECORDED);
	customMessage->encodeBody();
	customMessage->encodeData();
	customMessage->debug();

	sendMessage(*customMessage);
	sendFile(filePath, codeFile);
}

void Client::sendFile(std::string filePath, std::string codeFile) {
	std::string buffer = FileTools::readStringFromFile(filePath.c_str());

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

		dataWritten += rangeToRead.length;
	}
	std::cout << "FileTools::file send" << std::endl;
}

void Client::sendFile(FileAnnotation fileAnnotation) {
	sendFile(fileAnnotation._filePath, fileAnnotation._codeFile);
}

void Client::handle_write(const boost::system::error_code& ec) {
	if (this->_stopped)
		return;
	if (!ec) {
		//INFO: Wait 10 seconds before sending the next heartbeat.
		this->_heartbeat_timer.expires_from_now(boost::posix_time::seconds(10));
		//this->_heartbeat_timer.async_wait(boost::bind(&Client::start_write, this));
	}
	else {
		std::cout << "Error on heartbeat: " << ec.message() << std::endl;
		disconnect();
	}
}

void Client::check_deadline() {
	if (this->_stopped)
		return;

	if (this->_deadline.expires_at() <= deadline_timer::traits_type::now()) {
		this->_socket.close();
		this->_deadline.expires_at(boost::posix_time::pos_infin);
	}
	this->_deadline.async_wait(boost::bind(&Client::check_deadline, this));
}

/* CALLBACK FOR IPROTOCOL */

bool Client::loginHandler(MessageLogin &message)  {
	//INFO: never call
	return true;
}

bool Client::loginResultHandler(MessageLoginResult &message)  {
	//TODO: call delegate to notify the result login  [OK]
	std::cout << "Client::loginResultHandler: " << ((message._loginResult.state == VSP::OK) ? "SUCCESS" : "FAILED") << std::endl;// none

	bool result = (message._loginResult.state == VSP::OK) ? true : false;
	this->_isLogin = result;
	if (this->_delegate) {
		this->_delegate->loginResultHandler(result);
	}
	return true;
}

bool Client::logOutHandler(MessageLogOut &message) {
	if (this->_delegate) {
		this->_delegate->logOutHandler();
	}
	return true;
}

bool Client::plateHandler(MessagePlate &message)  {
	//TODO: notify the GUI that a plate was treated [OK]
	if (message._plate.state == VSP::LIVE) {
		if (this->_delegate) {
			this->_delegate->plateLiveHandler(message._plate.code_file);
		}
	}
	else if (message._plate.state == VSP::RECORDED) {
		//TODO: add the plate result to the GUI"  plate list
		if (this->_delegate) {
			//INFO: if db add plate result to db
			this->_delegate->plateRecordedHandler(message._plate.code_file);
		}
	}
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
		if (this->_delegate) {//INFO: notify the delegate that a file was received
			this->_delegate->fileHandler(message._file.code_file);
		}
	}
	message.clean();
	return true;
}

bool Client::unknowMessageHandler(Message &message) {
	if (this->_delegate) {
		return this->_delegate->unknowMessageHandler();
	}
	//INFO: do additional stuff if needed
	return true;
}

/* Message */

Message *Client::headerMessage(char bodyType) {
	Message *message = new Message();
	message->encodeHeader(bodyType);
	message->decodeHeader();
	return message;
}

/* IProtocolDelegate */

void Client::setDelegate(IClientDelegate *delegate) {
	this->_delegate = delegate;
}
