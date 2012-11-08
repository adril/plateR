#include "Client.hpp"

Client::Client(tcp::io_service& io_service) : _socket(io_service)
{
	_timestamp = 0.0;
	_protocol = new LProtocol;
	_protocol->_delegate = this;
	memset(_userInfo.login, 0, sizeof(Login));
	memcpy(_userInfo.login, "nameless", sizeof("nameless"));
}
Client::~Client()
{
	boost::system::error_code err;

    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
    _socket.close(err);
}

// Internal IO functions
void Client::start()
{
	 _socket.async_read_some(boost::asio::buffer(_input_buffer.buffer, BUF_SIZE),
		 boost::bind(&Client::handle_input, this,
		 boost::asio::placeholders::error,
		 boost::asio::placeholders::bytes_transferred));
}
void Client::read(const boost::system::error_code& error)
{
	_socket.async_read_some(boost::asio::buffer(_input_buffer.buffer, BUF_SIZE),
		boost::bind(&Client::handle_input, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}
void Client::writeFinish(const boost::system::error_code &error, uint8_t *data)
{
}
void Client::handle_output(uint8_t *data, int len)
{
	boost::asio::async_write(this->_socket,
		boost::asio::buffer(data, len),
		boost::bind(&Client::writeFinish, this,
		boost::asio::placeholders::error, data));
}
void Client::handle_input(const boost::system::error_code& error, size_t bytes_transferred)
{
	/* DATA TREATMENT */
	_input_buffer.offset += bytes_transferred;
	std::cout << _input_buffer.buffer << std::endl;
	read(error);
}

/* SEND / RECEIVE MESSAGE */

void Client::receive_message(Message &msg)
{
	/* Message TREATMENT */
	_protocol->receiveMessage(msg);
}
void Client::send_message(Message &msg)
{
	/* Message TREATMENT */
	this->handle_output(NULL, 0);
}

/* CALLBACK FOR PROTOCOL */
void Client::loginHandler(Message &message)
{
}
void Client::loginResultHandler(Message &message)
{
}
void Client::logOutHandler(Message &message)
{
}
void Client::plateHandler(Message &message)
{
}
void Client::fileHandler(Message &message)
{
}
void Client::unknowMessageHandler(Message &message)
{
}

/* GETTER */

tcp::socket& Client::getSocket()
{
	return _socket;
}
std::string	Client::getIpAsString() const
{
	return _socket.remote_endpoint().address().to_string();
}