#include "../include/MessageLoginResult.hh"

MessageLoginResult::MessageLoginResult(Message &other, char type, char state)
  : Message(other) {
  this->_loginResult.type = type;
  this->_loginResult.state = state;
}

MessageLoginResult::MessageLoginResult(Message &other) {
  std::cout  << "[MessageLoginResult] ------ (Message const& other)" << std::endl;

  /* copy of data */
  this->_header = new char[other.getHeaderLength()];
  this->copyString(other.getHeader(), this->getHeader(), other.getHeaderLength());
  //  std::memset(this->header_, '\0', other.getHeaderLength());
  //  std::memcpy(this->header_, other.getHeader(), other.getHeaderLength());

  this->_body = new char[other.getBodyLength()];
  this->copyString(other.getBody(), this->getBody(), other.getBodyLength());
  //  std::memset(this->_body, '\0', other.getBodyLength());
  //  std::memcpy(this->_body, other.getBody(), other.getBodyLength());

  /* copy of header infos */
  this->_type = other.getType();
  this->_bodyLength = other.getBodyLength();
}

MessageLoginResult::~MessageLoginResult() {}

void MessageLoginResult::encodeBody() {
  std::cout << "MessageLoginResult::encodeBody()" << std::endl;
  std::memcpy(this->_body, &_loginResult, sizeof(this->_loginResult));
}

void MessageLoginResult::encodeBodyWithError(char error) {
   this->_error = error;
   encodeBody();
}

void MessageLoginResult::decodeBody() {
  this->_loginResult.type = reinterpret_cast<VSP::LoginResult *>(this->_body)->type;
  this->_loginResult.state = reinterpret_cast<VSP::LoginResult *>(this->_body)->state;
}
