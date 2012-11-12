///#include "../include/MessageLoginResult.hh"
#include "MessageLoginResult.hh"

MessageLoginResult::MessageLoginResult(Message &other, char type, char state)
	: Message(other) {
		std::cout  << "MessageLoginResult::MessageLoginResult(Message &other, char type, char state)" << std::endl;
		this->_loginResult.type = type;
		this->_loginResult.state = state;
}

MessageLoginResult::MessageLoginResult(Message &other)
	: Message(other) {
		std::cout  << "MessageLoginResult::MessageLoginResult(Message &other)" << std::endl;
}

MessageLoginResult::~MessageLoginResult() {
	std::cout << "MessageLoginResult::~MessageLoginResult()" << std::endl;
}

void MessageLoginResult::encodeBody() {
	std::cout << "MessageLoginResult::encodeBody()" << std::endl;
	std::memcpy(this->_body, &_loginResult, sizeof(this->_loginResult));
}

void MessageLoginResult::decodeBody() {
	std::cout << "MessageLoginResult::decodeBody()" << std::endl;
	this->_loginResult.type = reinterpret_cast<VSP::LoginResult *>(this->_body)->type;
	this->_loginResult.state = reinterpret_cast<VSP::LoginResult *>(this->_body)->state;
}

void MessageLoginResult::debug() {
	std::cout << "MessageLoginResult::debug()" << std::endl;
	std::cout << "type: " << (int)_loginResult.type << " state: " << (int)_loginResult.state << std::endl;
}
