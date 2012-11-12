#include "Client.hpp"

Client::Client(tcp::io_service& io_service) : _socket(io_service) {
	_timestamp = 0.0;
	_protocol = new LProtocol(this);
}
Client::~Client() {
	boost::system::error_code err;
	_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
	_socket.close(err);
}

// Internal IO functions
void Client::start() {
	const boost::system::error_code error;
	readHeader(error);
}
void Client::readHeader(const boost::system::error_code& error) {
	this->_inputMessage = new Message();

	_socket.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getHeader(), this->_inputMessage->getHeaderLength()),
		boost::bind(&Client::readHeaderHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}
void Client::readBody(const boost::system::error_code& error) {
	_socket.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getBody(), this->_inputMessage->getBodyLength()),
		boost::bind(&Client::readBodyHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void Client::readHeaderHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	//TODO: do job on the message
	if (checkError(error))
		return;

	this->_inputMessage->decodeHeader();
	this->_inputMessage->debug();

	//TODO: if bad header -> 	readheader(error); and clean the Message
	readBody(error);
}

void Client::readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	if (checkError(error))
		return;
	this->_protocol->receiveMessage(*this->_inputMessage);
	//this->_inputMessage->clean();
	readHeader(error);
}

void Client::writeFinish(const boost::system::error_code &error, uint8_t *data) {
	//TODO: clean message
	if (data != NULL)
		delete[] data;
	if (checkError(error))
		return;
	else
      std::cout << "Client::handle_output. SUCESS !" << std::endl;
}

/* SEND / RECEIVE MESSAGE */

void Client::sendMessage(Message &message)
{
	boost::asio::async_write(this->_socket,
		boost::asio::buffer((unsigned char *)message.getData(), message.getDataLength()),
		boost::bind(&Client::writeFinish, this,
		boost::asio::placeholders::error, (unsigned char *)message.getData()));
}

/* CALLBACK FOR PROTOCOL */

//TODO: do the job and answer to the client

void Client::loginHandler(Message &message) {
	// check username and password in DB
	Message *answerMessage = this->headerMessage(VSP::LOGIN_RESULT);
	MessageLoginResult *customMessage = new MessageLoginResult(*answerMessage, VSP::LOGIN_RESULT, VSP::OK);
	customMessage->encodeBody();
	customMessage->encodeData();
	sendMessage(*customMessage);
}

void Client::loginResultHandler(Message &message) {
}

void Client::logOutHandler(Message &message) {

}

void Client::plateHandler(Message &message) {

}

void Client::fileHandler(Message &message) {

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

bool Client::checkError(const boost::system::error_code& error) {
	if (error) {
		if (this->_delegate) {
			//TODO: check the error value
			this->_delegate->disconectClient(this);
			delete this;
			return true;
		}
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

void Client::debug() {

}