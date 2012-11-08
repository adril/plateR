#include "../include/MessageLogin.hh"

MessageLogin::MessageLogin(Message &other, char *login, char *password)
  : Message(other) {

  /* copy of header infos */
  this->_type = other.getType();
  this->_bodyLength = other.getBodyLength();

  this->copyString(login, this->_login.login, VSP::LOGIN_SIZE);
  this->copyString(password, this->_login.password, VSP::PASSWORD_SIZE);
}

MessageLogin::MessageLogin(Message &other) {
  std::cout  << "[MessageLogin] ------ (Message const& other)" << std::endl;

  /* copy of data */
  this->_header = new char[other.getHeaderLength()];
  this->copyString(other.getHeader(), this->getHeader(), other.getHeaderLength());
  //  std::memset(this->_header, '\0', other.getHeaderLength());
  //  std::memcpy(this->_header, other.getHeader(), other.getHeaderLength());

  this->_body = new char[other.getBodyLength()];
  this->copyString(other.getBody(), this->getBody(), other.getBodyLength());
  //  std::memset(this->_body, '\0', other.getBodyLength());
  //  std::memcpy(this->_body, other.getBody(), other.getBodyLength());

  /* copy of header infos */
  this->_type = other.getType();
  this->_bodyLength = other.getBodyLength();

}

MessageLogin::~MessageLogin() {
  std::cout << "[MessageLogin] ~MessageLogin" << std::endl;
}

void MessageLogin::encodeBody() {
  std::cout << "MessageLogin::encodeBody" << std::endl;
  std::memcpy(this->_body, &this->_login, sizeof(this->_login));
}

void MessageLogin::encodeBodyWithError(char error) {
  this->_error = error;
  encodeBody();
}

void MessageLogin::decodeBody() {
  this->_login.type = reinterpret_cast<VSP::Login *>(this->_body)->type;
  //  this->_login.login = reinterpret_cast<VSP::Login *>(this->_body)->login;
  //this->_login.password = reinterpret_cast<VSP::Login *>(this->_body)->password;
}
