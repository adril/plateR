#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "LProtocol.hh"
#include "Common.hpp"


class Client : public IProtocolDelegate
{

public:
	Client(tcp::io_service& io_service);
	~Client();

	void start();
	tcp::socket& getSocket();
    std::string	getIpAsString() const;

	/* CALLBACK FOR LPROTOCOL */
	virtual void loginHandler(Message &message);
	virtual void loginResultHandler(Message &message);
	virtual void logOutHandler(Message &message);
	virtual void plateHandler(Message &message);
	virtual void fileHandler(Message &message);
	virtual void unknowMessageHandler(Message &message);
private :
	tcp::socket		_socket;
	LProtocol	*_protocol;
	buffer_t	_input_buffer;
	buffer_t	_output_buffer;  
	userInfo	_userInfo;
	double		_timestamp;

	void handle_input();

	// Internal IO functions
	void read(const boost::system::error_code& error);
	void writeFinish(const boost::system::error_code &error, uint8_t *data);
	void handle_output(uint8_t*, int);
	void handle_input(const boost::system::error_code& error, size_t bytes_transferred);


	void receive_message(Message &msg);
	void send_message(Message &msg);
};

#endif