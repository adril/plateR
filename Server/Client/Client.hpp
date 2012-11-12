#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "LProtocol.hh"
#include "Common.hpp"
#include "IServerDelegate.hpp"
#include "VSP.h"

class Client : public IProtocolDelegate {
private :
	tcp::socket	 _socket;
	LProtocol	*_protocol;
	Message		*_inputMessage;
	MessageLogin *_userInfo;	
	double		_timestamp;
	IServerDelegate *_delegate;


public:
	Client(tcp::io_service& io_service);
	~Client();

	void start();
	tcp::socket& getSocket();
	std::string	getIpAsString() const;
	void debug();
	void displayUserInfo() const;

	//INFO: setter
	void setDelegate(IServerDelegate *delegate);


	/* CALLBACK FOR LPROTOCOL */
	virtual void loginHandler(MessageLogin &message);
	virtual void loginResultHandler(MessageLoginResult &message);
	virtual void logOutHandler(MessageLogOut &message);
	virtual void plateHandler(MessagePlate &message);
	virtual void fileHandler(MessageFile &message);
	virtual void unknowMessageHandler(Message &message);
private:
	void disconect();

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