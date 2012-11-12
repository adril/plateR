#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "LProtocol.hh"
#include "Common.hpp"
#include "IServerDelegate.hpp"

class Client : public IProtocolDelegate
{

public:
	Client(tcp::io_service& io_service);
	~Client();

	void start();
	tcp::socket& getSocket();
	std::string	getIpAsString() const;
	void debug();

	//INFO: setter
	void setDelegate(IServerDelegate *delegate);


	/* CALLBACK FOR LPROTOCOL */
	virtual void loginHandler(Message &message);
	virtual void loginResultHandler(Message &message);
	virtual void logOutHandler(Message &message);
	virtual void plateHandler(Message &message);
	virtual void fileHandler(Message &message);
	virtual void unknowMessageHandler(Message &message);
private :
	tcp::socket	 _socket;
	LProtocol	*_protocol;
	Message		*_inputMessage;
	buffer_t	_input_buffer;
	buffer_t	_output_buffer;  
	userInfo	_userInfo;
	double		_timestamp;
	IServerDelegate *_delegate;

	void handle_input();

	//INFO: new read methodes
	void readHeader(const boost::system::error_code& error);
	void readBody(const boost::system::error_code& error);

	//INFO: new handler read methodes
	void readHeaderHandler(const boost::system::error_code& error, size_t bytes_transferred);
	void readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred);

	bool checkError(const boost::system::error_code& error);

	// Internal IO functions
	void writeFinish(const boost::system::error_code &error, uint8_t *data);
	
	void sendMessage(Message &msg);
	//INFO: Message
	Message *headerMessage(char bodyType);
};

#endif