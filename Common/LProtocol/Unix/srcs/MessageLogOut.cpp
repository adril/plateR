#include "../include/MessageLogOut.hh"

MessageLogOut::MessageLogOut(Message &other, char type)
  : Message(other) {
  this->_logOut.type = type;
}

MessageLogOut::MessageLogOut(Message &other)
	: Message(other) {
  std::cout  << "[MessageLogOut] ------ (Message const& other)" << std::endl;
  /*
  // copy of data
  this->_header = new char[other.getHeaderLength()];
  this->copyString(other.getHeader(), this->getHeader(), other.getHeaderLength());
  //  std::memset(this->header_, '\0', other.getHeaderLength());
  //  std::memcpy(this->header_, other.getHeader(), other.getHeaderLength());

  this->_body = new char[other.getBodyLength()];
  this->copyString(other.getBody(), this->getBody(), other.getBodyLength());
  //  std::memset(this->_body, '\0', other.getBodyLength());
  //  std::memcpy(this->_body, other.getBody(), other.getBodyLength());

  /// copy of header infos
  this->_type = other.getType();
  this->_bodyLength = other.getBodyLength();
  */
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