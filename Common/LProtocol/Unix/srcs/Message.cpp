#include "../include/Message.hh"

Message::Message() {
  std::cout  << "[Message]" << std::endl;

  this->initDataTypeSizeMap();
  this->_error = VSP::ERROR_NOTHING;
  this->_header = new char[this->getHeaderLength()];
}

Message::Message(Message const& other) {
  std::cout  << "[Message] (Message const& other)" << std::endl;

  this->initDataTypeSizeMap();
  this->_error = VSP::ERROR_NOTHING;

  /* copy of data */
  this->_header = new char[other.getHeaderLength()];
  std::memset(this->_header, '\0', other.getHeaderLength());
  std::memcpy(this->_header, other.getHeader(), other.getHeaderLength());

  //  this->_header = other.getHeader();
  //this->_header = other.getHeader();
  this->_body = other.getBody();
  this->_data = other.getData();

  /* copy of header infos */
  this->_type = other.getType();
  this->_bodyLength = other.getBodyLength();
  //  this->_login = other.getLogin();
  this->_error = other._error;
}

Message::~Message() {
  std::cout << "[Message] ~Message"<< std::endl;
  //  this->sendList_.erase(this->sendList_.begin(), this->sendList_.end());
  // delete[] this->body_;
  // delete[] this->_header;
  // delete[] this->data_;
}

void Message::initDataTypeSizeMap() {
  this->_dataTypeSizeMap[VSP::LOGIN] = sizeof(VSP::Login);
  this->_dataTypeSizeMap[VSP::LOGIN_RESULT] = sizeof(VSP::LoginResult);
  this->_dataTypeSizeMap[VSP::LOGOUT] = sizeof(VSP::LogOut);
  this->_dataTypeSizeMap[VSP::PLATE] = sizeof(VSP::Plate);
  this->_dataTypeSizeMap[VSP::FILE] = sizeof(VSP::File);
}

char *Message::getHeader() const {
  return this->_header;
}

char *Message::getBody() const {
  return this->_body;
}

char *Message::getData() const {
  return this->_data;
}

size_t Message::getHeaderLength() const {
  return sizeof(VSP::Header);
}

size_t Message::getBodyLength() const {
  return this->_bodyLength;
}

size_t Message::getDataLength() const {
  return this->getHeaderLength() + this->getBodyLength();//_bodyLength;
}

char Message::getType() const {
  return this->_type;
}

void Message::setError(char error) {
  this->_error = error;
}

char Message::getError() const {
  return this->_error;
}

void Message::encodeHeader(char type) {
  std::cout << "[Message] encodeHeader" << std::endl;

  VSP::Header tmp;

  tmp.type = type;
  tmp.body_size = this->_dataTypeSizeMap[type];//this->getHeaderLength();
  tmp.error = VSP::ERROR_NOTHING;

  std::memset(this->_header, '\0', this->getHeaderLength());
  std::memcpy(this->_header, &tmp, this->getHeaderLength());
}

void Message::encodeHeader() {
  std::cout << "[Message] encodeHeader" << std::endl;

  VSP::Header tmp;

  tmp.type = this->_type;
  tmp.body_size = this->_dataTypeSizeMap[this->_type];//this->getHeaderLength();
  tmp.error = this->_error;

  std::memset(this->_header, '\0', this->getHeaderLength());
  std::memcpy(this->_header, &tmp, this->getHeaderLength());
}

void Message::encodeBody() {
  std::cout << "[Message] encodeBody -> neverCall"<< std::endl;
  //to impl in inherit obj
}

void Message::decodeBody() {
  std::cout << "[Message] decodeBody -> neverCall"<< std::endl;
}

void Message::encodeBodyWithError(char error) {
  (void)error;
  std::cout << "[Message] decodeBodyWithError -> neverCall"<< std::endl;
}

void Message::encodeData() {
  this->_data = new char[this->getHeaderLength() + this->getBodyLength()];//this->bodyLength_];
  std::memcpy(this->_data, this->_header, this->getHeaderLength());
  std::memcpy(this->_data + this->getHeaderLength(), this->_body, this->getBodyLength());//this->bodyLength_);
}

void Message::decodeHeader() {
  this->_type = reinterpret_cast<VSP::Header *>(this->_header)->type;
  this->_bodyLength = this->_dataTypeSizeMap[this->_type];
  //this->_login = reinterpret_cast<VSP::Header *>(this->_header)->login_from;
  //init body
  this->_body = new char[this->getBodyLength()];//this->bodyLength_);
  std::memset(this->_body, '\0', this->getBodyLength());//this->bodyLength_);
}
//INFO: tools

void Message::copyString(char *src, char *dest, size_t size) {
  std::memset(dest, '\0', size);
  std::strcpy(dest, src);
}

void Message::debug() {
  std::cout << "[Message] aff" << std::endl;
  std::cout << "[Message] body size: " << this->_bodyLength << " and type " << (int)this->_type << " error: " << (int)this->_error << std::endl;
}

#ifdef _WIN32
extern "C" __declspec(dllexport) Message* __cdecl getNewMessage()
#elif __unix__
extern "C" Message *getNewMessage()
#endif
{
    return (new Message());
}
