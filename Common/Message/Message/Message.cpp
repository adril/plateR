// This is the main DLL file.

#include "stdafx.h"
#include "Message.h"

//#include "../include/Message.hpp"

Message::Message() {
  this->data_ = new char[sizeof(Header) + 1];
}

Message::Message(Message const &other) {
  data_ = other.getData();
  bodyLength_ = other.getBodyLength();
  login_ = new char[LOGIN_SIZE];
  memset(login_, '\0', LOGIN_SIZE);
  strcpy(login_, other.getLogin());
  type_ = other.getType();
}

Message::~Message() {
  // delete data_;
}

char * Message::getLogin() const {
  return login_;
}

char* Message::getData() const {
  return data_;
}

char Message::getType() const {
  return this->type_;
}

size_t Message::getBodyLength() const {
  return this->bodyLength_;
}

size_t Message::getLength() const {
    return this->bodyLength_ + sizeof(Header);
}

/* Header's functions */

void Message::decodeHeader() {
  bodyLength_ = ((Header*)data_)->size_struct;
  type_ = ((Header*)data_)->command;
  login_ = new char[LOGIN_SIZE];
  std::memset(login_, '\0', LOGIN_SIZE);
  std::strcpy(login_, ((Header*)data_)->login_from);
  delete[] data_;
  data_ = new char[bodyLength_ + 1];
}

void Message::encodeHeader(char type, const char *login) {
  int assoType[4] = { sizeof(Connection), sizeof(Friend), sizeof(Txt), sizeof(Call) };
  Header tmp;

  tmp.command = type;
  tmp.size_struct = assoType[(int)type];
  std::memset(tmp.login_from, '\0', LOGIN_SIZE);
  std::strcpy(tmp.login_from, login);
  delete[] this->data_;
  login_ = new char[LOGIN_SIZE];
  std::memset(login_, '\0', LOGIN_SIZE);
  std::strcpy(login_, login);
  this->data_ = new char[sizeof(Header) + assoType[(int)type] + 1];
  std::memcpy(this->data_, &tmp, sizeof(tmp));
}

/* ******************* */

/* Body's Functions */

void Message::encodeBody() {
}