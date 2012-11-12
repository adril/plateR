#include "../include/MessageLogOut.hh"

MessageLogOut::MessageLogOut(Message &other, char type)
	: Message(other) {
		this->_logOut.type = type;
}

MessageLogOut::MessageLogOut(Message &other)
	: Message(other) {
		std::cout  << "[MessageLogOut] ------ (Message const& other)" << std::endl;
}

MessageLogOut::~MessageLogOut() {}

void MessageLogOut::encodeBody() {
	std::cout << "MessageLogOut::encodeBody()" << std::endl;
	std::memcpy(this->_body, &this->_logOut, sizeof(this->_logOut));
}

void MessageLogOut::decodeBody() {
	this->_logOut.type = reinterpret_cast<VSP::LogOut *>(this->_body)->type;
}

void MessageLogOut::debug() {
	std::cout << "MessageLogOut::debug()" << std::endl;
	std::cout << "type: " << (int)this->_logOut.type << std::endl;
}