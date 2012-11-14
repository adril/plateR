#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <iostream>
#include <string>
#include <list>
#include <fstream>


#include <list>
#include "LProtocol.hh"
#include "Common.hpp"
#include "IServerDelegate.hpp"
#include "VSP.h"
#include "FileAnnotation.h"
#include "PlateAnnotation.h"

class Client : public IProtocolDelegate {
private :
	tcp::socket	_socket;
	LProtocol *_protocol;
	Message	*_inputMessage;
	MessageLogin *_userInfo;
	double	_timestamp;

	IServerDelegate *_delegate;

	tcp::io_service	_io_service;
	std::string	_fileBuffer;

	boost::asio::deadline_timer	_getUpdatedTimer;

	std::list<Message *> _sendMessageList;
	std::list<FileAnnotation> _sendFileList;
	std::list<PlateAnnotation> _plateToTreatList;
	bool _isLogin;
public:
	Client(tcp::io_service& io_service);
	~Client();

	void start();

	/* CALLBACK FOR LPROTOCOL */
	virtual bool loginHandler(MessageLogin &message);
	virtual bool loginResultHandler(MessageLoginResult &message);
	virtual bool logOutHandler(MessageLogOut &message);
	virtual bool plateHandler(MessagePlate &message);
	virtual bool fileHandler(MessageFile &message);
	virtual bool unknowMessageHandler(Message &message);

	/* SERVER OUTPUT */
	void sendRecordedPlate();
	void sendInfoPlate(std::string codeFile, char state);
	void sendLogout();
	void sendFile(std::string filePath);

	/* HANDLER */
	void getUpdatedHandler(const boost::system::error_code& error);

	/* GETTER */
	tcp::socket& getSocket();
	std::string	getIpAsString() const;

	/* Setter */
	void setDelegate(IServerDelegate *delegate);

	/* Misc */
	void debug();
	void displayUserInfo() const;

private:
	void disconect();

	/* READ */ 
	void readHeader(const boost::system::error_code& error);
	void readBody(const boost::system::error_code& error);

	/* READ HANDLER */
	void readHeaderHandler(const boost::system::error_code& error, size_t bytes_transferred);
	void readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred);

	/* Internal IO functions */
	void writeFinish(const boost::system::error_code &error, uint8_t *data);
	bool checkError(const boost::system::error_code& error);

	/* MESSAGE */
	void sendMessage(Message &msg);
	Message *headerMessage(char bodyType);
};

#endif /* _CLIENT_HPP_ */