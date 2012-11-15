#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <iostream>
#include <fstream>
#include <queue>

#include "LProtocol.hh"
#include "VSP.h"

#include "AppData.hpp"
#include "Tools.hpp"
#include "FileTools.hpp"
#include "IClientDelegate.hh"
#include "FileAnnotation.h"

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

class Client : public IProtocolDelegate {
private:
	bool _isLogin;
	tcp::socket _socket;
	boost::asio::streambuf _input_buffer;
	std::string _fileBuffer;
	deadline_timer _deadline;
	deadline_timer _heartbeat_timer;
	deadline_timer	_getUpdatedTimer;
	size_t _getUpdatedInterval;
	std::list<FileAnnotation> _sendFileList;;
	IClientDelegate *_delegate;

public:
	Client(boost::asio::io_service& io_service);
	~Client();

	Message *_inputMessage;
	LProtocol	*_protocol;

	void start(tcp::resolver::iterator endpoint_iter);
	void disconnect();

	/* set delegate Pointer to be nottified */
	void setDelegate(IClientDelegate *delegate);

		/* SEND */

	void sendMessage(Message &msg);
	void sendMessageQueue(std::queue<Message*> msgQueue);
	void sendLogin(std::string username, std::string password);
	void sendLogout();
	void sendRecognizePlate(std::string filePath, std::string codeFile, std::string codePlate);//INFO: recognize a plate
	void sendFile(std::string filePath, std::string codeFile);
	void sendFile(FileAnnotation fileAnnotation);


private:
	void startConnect(tcp::resolver::iterator endpoint_iter);
	void handleConnect(const boost::system::error_code& ec, tcp::resolver::iterator endpoint_iter);
	void startRead();

	/* READ  */

	void read(const boost::system::error_code& error);
	void readHeader(const boost::system::error_code& error);
	void readBody(const boost::system::error_code& error);

	void readHeaderHandler(const boost::system::error_code& error, size_t bytes_transferred);
	void readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred);


	void handle_write(const boost::system::error_code& ec);
	void check_deadline(const boost::system::error_code& error);

	/* HANDLER */
	void getUpdatedHandler(const boost::system::error_code& error);

	virtual bool loginHandler(MessageLogin &message);
	virtual bool loginResultHandler(MessageLoginResult &message);
	virtual bool logOutHandler(MessageLogOut &message);
	virtual bool plateHandler(MessagePlate &message);
	virtual bool fileHandler(MessageFile &message);
	virtual bool unknowMessageHandler(Message &message);
	Message *headerMessage(char bodyType);

};

#endif
