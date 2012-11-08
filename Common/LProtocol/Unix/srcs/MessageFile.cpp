#include "../include/MessageFile.hh"

MessageFile::MessageFile(Message &other, char type, size_t indx, size_t max_indx, char *file_name, size_t to_read, char *file)
  : Message(other) {
  this->_file.type = type;
  this->_file.indx = indx;

  //  std::memset(this->_file.file_name, '\0', RVSP::NAME_SIZE);
  // std::strcpy(this->_file.file_name, file_name);
  this->copyString(file_name, this->_file.file_name, VSP::NAME_SIZE);

  this->_file.max_indx = max_indx;
  this->_file.to_read = to_read;

  //  std::memset(this->_file.file, '\0', RVSP::FILE_SIZE);
  //  std::strcpy(this->_file.file, file);
  this->copyString(file, this->_file.file, VSP::FILE_SIZE);
}

MessageFile::MessageFile(Message &other) {
  std::cout  << "[MessageFile] ------ (Message const& other)" << std::endl;

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

MessageFile::~MessageFile() {}

void MessageFile::encodeBody() {
  std::cout << "MessageFile::encodeBody()" << std::endl;
  std::memcpy(this->_body, &this->_file, sizeof(this->_file));
}

void MessageFile::encodeBodyWithError(char error) {
  std::cout << "MessageFile::encodeBody()" << std::endl;
  this->_error = error;
  encodeBody();
}

void MessageFile::decodeBody() {
  this->_file.type = reinterpret_cast<VSP::File *>(this->_body)->type;
  this->_file.indx = reinterpret_cast<VSP::File *>(this->_body)->indx;

  //  this->_file.file_name = reinterpret_cast<VSP::File *>(this->_body)->file_name;
  //this->_file.file = reinterpret_cast<VSP::File *>(this->_body)->file;
  //  std::memset(this->_file.file_name, '\0', RVSP::NAME_SIZE);
  //  std::strcpy(this->_file.file_name, reinterpret_cast<VSP::File *>(this->_body)->file_name);
  this->_file.to_read = reinterpret_cast<VSP::File *>(this->_body)->to_read;
  //  std::memset(this->_file.file, '\0', RVSP::FILE_SIZE);
  //  std::strcpy(this->_file.file, reinterpret_cast<VSP::File *>(this->_body)->file);
}
