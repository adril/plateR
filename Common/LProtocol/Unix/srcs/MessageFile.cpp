#include "../include/MessageFile.hh"

MessageFile::MessageFile(Message &other, char type, size_t indx, size_t max_indx, char *file_name, size_t to_read, char *file)
  : Message(other) {
  this->_file.type = type;
  this->_file.indx = indx;
  this->_file.max_indx = max_indx;

  this->copyString(file_name, this->_file.file_name, VSP::NAME_SIZE);

  this->_file.to_read = to_read;

  this->copyString(file, this->_file.file, VSP::FILE_SIZE);
}

MessageFile::MessageFile(Message &other)
	: Message(other) {
  std::cout  << "[MessageFile] ------ (Message const& other)" << std::endl;
 }

MessageFile::~MessageFile() {}

void MessageFile::encodeBody() {
  std::cout << "MessageFile::encodeBody()" << std::endl;
  std::memcpy(this->_body, &this->_file, sizeof(this->_file));
}

void MessageFile::decodeBody() {
  this->_file.type = reinterpret_cast<VSP::File *>(this->_body)->type;
  this->_file.indx = reinterpret_cast<VSP::File *>(this->_body)->indx;
  this->_file.max_indx = reinterpret_cast<VSP::File *>(this->_body)->max_indx;
  this->_file.to_read = reinterpret_cast<VSP::File *>(this->_body)->to_read;

  this->copyString(reinterpret_cast<VSP::File *>(this->_body)->file_name, this->_file.file_name, VSP::NAME_SIZE);
  this->copyString(reinterpret_cast<VSP::File *>(this->_body)->file, this->_file.file, VSP::FILE_SIZE/*this->_file.to_read*/);
}

void MessageFile::debug() {
	std::cout << "MessageLogOut::debug()" << std::endl;
	std::cout << "type: " << (int)this->_file.type << " indx: " << this->_file.indx << " max_indx: " << this->_file.max_indx << " file_name: " << this->_file.file_name
		<< " to_read: " << this->_file.to_read << " file: " << this->_file.file << std::endl;
}
