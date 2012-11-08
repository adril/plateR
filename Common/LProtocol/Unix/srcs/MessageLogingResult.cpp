#include "../include/MessageLoginResult.hh"

MessageLoginResult::MessageLoginResult(Message &other, char type, char state)
  : Message(other) {
  this->_loginResult.type = type;
  this->_loginResult.state = state;
}

MessageLoginResult::MessageLoginResult(Message &other) {
  std::cout  << "[MessageLoginResult] ------ (Message const& other)" << std::endl;

  /* copy of data */
  this->header_ = new char[other.getHeaderLength()];
  this->copyString(other.getHeader(), this->getHeader(), other.getHeaderLength());
  //  std::memset(this->header_, '\0', other.getHeaderLength());
  //  std::memcpy(this->header_, other.getHeader(), other.getHeaderLength());

  this->body_ = new char[other.getBodyLength()];
  this->copyString(other.getBody(), this->getBody(), other.getBodyLength());
  //  std::memset(this->body_, '\0', other.getBodyLength());
  //  std::memcpy(this->body_, other.getBody(), other.getBodyLength());

  /* copy of header infos */
  this->_type = other.getType();
  this->_bodyLength = other.getBodyLength();
}

MessageLoginResult::~MessageLoginResult() {}

void MessageLoginResult::encodeBody() {
  std::cout << "MessageLoginResult::encodeBody()" << std::endl;
  std::memcpy(this->body_, &_loginResult, sizeof(this->_loginResult));
}

void MessageLoginResult::encodeBodyWithError(char error) {
   this->_loginResult.error = error;
   encodeBody();
}

void MessageLoginResult::decodeBody() {
  this->_loginResult.command = reinterpret_cast<VSP::Game *>(this->body_)->command;
  std::memset(this->_loginResult.file, '\0', VSP::NAME_SIZE);
  std::strcpy(this->_loginResult.file, reinterpret_cast<VSP::Game *>(this->body_)->file);
  this->_loginResult.x = reinterpret_cast<VSP::Game *>(this->body_)->x;
  this->_loginResult.y = reinterpret_cast<VSP::Game *>(this->body_)->y;
  this->_loginResult.type = reinterpret_cast<VSP::Game *>(this->body_)->type;
  this->_loginResult.id = reinterpret_cast<VSP::Game *>(this->body_)->id;
  this->_loginResult.error = reinterpret_cast<VSP::Game *>(this->body_)->error;
}
