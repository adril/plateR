#pragma once

#include "Client.hpp"
#include "AppData.hpp"
#include "IClientDelegate.hh"
#include "StringTools.hpp"

class ClientController : IClientDelegate {
private:
	Client *_client;

	std::string _login;
	bool _isConnected;
	bool _isLogin;
	deadline_timer _taskManagerTimer;
	size_t _taskManagerInterval;
	boost::asio::io_service _io_service;
	tcp::resolver _resolver;
public:
	ClientController(boost::asio::io_service &io_service);
	~ClientController(void);

	void start();
	virtual void connectHandler(bool result);
	virtual void loginResultHandler(bool result);
	virtual void logOutHandler();
	virtual void plateLiveHandler(std::string codeFile, std::string codePlate, std::string plateValue);
	virtual void plateRecordedHandler(std::string codeFile, std::string codePlate, std::string plateValue);
	virtual void fileHandler(std::string codeFile) ;
	virtual bool unknowMessageHandler();

	void recognizePlate(std::string filePath);
	void taskManagerHandler(const boost::system::error_code& error);

};

