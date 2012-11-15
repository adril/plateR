#include "ClientController.h"

ClientController::ClientController(boost::asio::io_service &io_service) 
	:  _taskManagerTimer(io_service), _resolver(io_service) {
		this->_taskManagerInterval = 10;
		this->_login = "julien";
		this->_isConnected = false;
		this->_isLogin = false;

		this->_client = new Client(io_service);
		this->_client->setDelegate(this);

		this->_taskManagerTimer.expires_from_now(boost::posix_time::seconds(this->_taskManagerInterval));
		this->_taskManagerTimer.async_wait(boost::bind(&ClientController::taskManagerHandler, this, _1));
}

ClientController::~ClientController(void) {
	this->_taskManagerTimer.cancel();
}

void ClientController::start() {
	std::string host = AppData::getInstance()._host;
	std::string port = AppData::getInstance()._port;
	this->_client->start(this->_resolver.resolve(tcp::resolver::query(host, port)));
}

void ClientController::recognizePlate(std::string filePath) {//INFO: may be give filePath
	static int index;


	std::string filePathMd5 = StringTools::stringToMd5(filePath);
	std::cout << "filePathMd5: " << filePathMd5 << std::endl;
	std::string codeFile = "codeFile-" + this->_login + "-" + StringTools::integerToString(index);
	std::string codePlate = "codePlate-" + this->_login + "-" + StringTools::integerToString(index);

	this->_client->sendRecognizePlate(filePath, codeFile, codePlate);
	index++;
}

void ClientController::taskManagerHandler(const boost::system::error_code& error) {
	if (error == boost::asio::error::operation_aborted)
		return;
	//static int loged;

	if (this->_isConnected == true && this->_isLogin == true) {
		//INFO: do stuff if needed
		std::string filePath = AppData::getInstance()._appDirectoryPath + "/001.jpg";
		recognizePlate(filePath);
		///this->_taskManagerInterval = 10;
	} else {
		start();
	}
	_taskManagerTimer.expires_from_now(boost::posix_time::seconds(this->_taskManagerInterval));
	_taskManagerTimer.async_wait(boost::bind(&ClientController::taskManagerHandler, this, _1));
}

/* IClientDelegate */

void ClientController::connectHandler(bool result) {
	std::cout << "connectHandler" << std::endl;
	this->_isConnected = result;
	//TODO: send login request
	if (this->_isConnected == true)
		this->_client->sendLogin(this->_login, "password");
}

void ClientController::loginResultHandler(bool result) {
	std::cout << "loginResultHandler" << std::endl;
	std::cout << "result: " << (result ? "SUCCESS" : "FAILED") << std::endl;

	this->_isLogin = result;
	//TODO: notify GUI
}

void ClientController::logOutHandler() {
	std::cout << "logOutHandler" << std::endl;

	this->_isLogin = false;
	//TODO: notify GUI
}

void ClientController::plateLiveHandler(std::string codeFile, std::string codePlate, std::string plateValue) {
	std::cout << "plateLiveHandler" << std::endl;
	std::cout << "codeFile: " << codeFile << " codePlate: " << codePlate << " plateValue: " << plateValue << std::endl;

	//TODO: check if the receive plate is one asked
	//		display the plate value
}

void ClientController::plateRecordedHandler(std::string codeFile, std::string codePlate, std::string plateValue) {
	std::cout << "plateRecordedHandler" << std::endl;
	std::cout << "codeFile: " << codeFile << " codePlate: " << codePlate << " plateValue: " << plateValue << std::endl;

	//TODO: push the plate to the plate List
}

void ClientController::fileHandler(std::string codeFile){
	std::cout << "fileHandler" << std::endl;
	std::cout << "codeFile: " << codeFile << std::endl;

	//TODO: receive a file check if this file was asked live or recorded
}

bool ClientController::unknowMessageHandler() {
	std::cout << "unknowMessageHandler" << std::endl;
	return true;
}
