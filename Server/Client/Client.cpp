#include "Client.hpp"

Client::Client(tcp::io_service& io_service) : _socket(io_service) {
	_timestamp = 0.0;
	_protocol = new LProtocol;
	_protocol->_delegate = this;
	_inputMessage = new Message;

	//debug();

	//memset(_userInfo.login, 0, sizeof(Login));
	//memcpy(_userInfo.login, "nameless", sizeof("nameless"));
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

	//_socket.async_read_some(boost::asio::buffer(_input_buffer.buffer, BUF_SIZE),
	// boost::bind(&Client::handle_input, this,
	// boost::asio::placeholders::error,
	// boost::asio::placeholders::bytes_transferred));
}
void Client::read(const boost::system::error_code& error) {
	_socket.async_read_some(boost::asio::buffer(_input_buffer.buffer, BUF_SIZE),
		boost::bind(&Client::handle_input, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
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
	//this->_inputMessage->decodeBody();
	this->_protocol->receiveMessage(*this->_inputMessage);
	//this->_inputMessage->clean();
	readHeader(error);
}

void Client::writeFinish(const boost::system::error_code &error, uint8_t *data) {
	//TODO: clean message
	if (checkError(error))
		return;

}
//INFO: out of business
void Client::handle_output(uint8_t *data, int len)
{
	boost::asio::async_write(this->_socket,
		boost::asio::buffer(data, len),
		boost::bind(&Client::writeFinish, this,
		boost::asio::placeholders::error, data));
}
void Client::handle_input(const boost::system::error_code& error, size_t bytes_transferred)
{
	//INFO: out of business

	/* DATA TREATMENT */
	_input_buffer.offset += bytes_transferred;
	std::cout << _input_buffer.buffer << std::endl;
	read(error);
}
//INFO: end ofOUT of business
/* SEND / RECEIVE MESSAGE */

void Client::sendMessage(Message &message)
{
	boost::asio::async_write(this->_socket,
		boost::asio::buffer((unsigned char *)message.getData(), message.getDataLength()),
		boost::bind(&Client::writeFinish, this,
		boost::asio::placeholders::error, (unsigned char *)message.getData()));
	return;
	//INFO: out of business
	/* Message TREATMENT */
	//this->handle_output(NULL, 0);
}

/* CALLBACK FOR PROTOCOL */

//TODO: do the job and answer to the client

void Client::loginHandler(Message &message) {

	message.debug();
	///((MessageLogin)message).debug();

	Message *answerMessage = this->headerMessage(VSP::LOGIN_RESULT);
	MessageLoginResult *customMessage = new MessageLoginResult(*answerMessage, VSP::LOGIN_RESULT, VSP::OK);
	customMessage->encodeBody();
	//std::cout << "message->getBody(): " << message->getBody() << std::endl;
	customMessage->encodeData();
	//std::cout << "message->getData(): " << customMessage->getData() << std::endl;
	customMessage->debug();
	//debugMessage(*customMessage);

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
	//std::cout << "message->getHeaderLength(): " << message->getHeaderLength() << std::endl;
	message->encodeHeader(bodyType);
	message->decodeHeader();//INFO: alloc body
	//std::cout << "message->getBodyLength(): " << message->getBodyLength() << std::endl;

	//std::cout << "message->getHeader(): " << message->getHeader() << std::endl;
	//message->debug();
	return message;
}

/* ERROR */

bool Client::checkError(const boost::system::error_code& error) {
	if (error) {
		if (this->_delegate) {
			//TODO: check the error value
			delete this;
			return true;
			this->_delegate->disconectClient(this);
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
	std::cout	<< "[LProtocol] debug" << std::endl;
	{
		Message *message = new Message;
		message->encodeHeader(VSP::LOGIN_RESULT);
		message->decodeHeader();
		message->getHeader();
		message->debug();

		MessageLoginResult *customMessage = new MessageLoginResult(*message, VSP::LOGIN_RESULT, VSP::KO);
		customMessage->encodeBody();

		customMessage->encodeData();

		customMessage->debug();


		std::cout << "header: " << message->getHeader() << std::endl;
	}
}