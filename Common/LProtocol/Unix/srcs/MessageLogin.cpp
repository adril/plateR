#include "../include/MessageLogin.hh"

MessageLogin::MessageLogin(Message &other, char *login, char *password)
	: Message(other) {
		this->copyString(login, this->_login.login, VSP::LOGIN_SIZE);
		this->copyString(password, this->_login.password, VSP::PASSWORD_SIZE);
}

MessageLogin::MessageLogin(Message &other)
	: Message(other) {
		std::cout  << "MessageLogin::MessageLogin(Message &other)" << std::endl;
}

MessageLogin::~MessageLogin() {
	std::cout << "MessageLogin::~MessageLogin()" << std::endl;
}

void MessageLogin::encodeBody() {
	std::cout << "MessageLogin::encodeBody" << std::endl;
	std::memcpy(this->_body, &this->_login, sizeof(this->_login));
}

void MessageLogin::decodeBody() {
	this->copyString(reinterpret_cast<VSP::Login *>(this->_body)->login, this->_login.login, VSP::LOGIN_SIZE);
	this->copyString(reinterpret_cast<VSP::Login *>(this->_body)->password, this->_login.password, VSP::PASSWORD_SIZE);
	this->_login.type = reinterpret_cast<VSP::Login *>(this->_body)->type;
}

void MessageLogin::debug() {
	std::cout << "MessageLogin::debug()" << std::endl;
	std::cout << "login: " << _login.login << " password: " << _login.password << std::endl;

}
