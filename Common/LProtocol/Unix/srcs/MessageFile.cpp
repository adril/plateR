#include "../include/MessageFile.hh"

MessageFile::MessageFile(Message &other, char type, size_t indx, size_t max_indx, char *code_file, size_t to_read, char *file)
  : Message(other) {
  this->_file.type = type;
  this->_file.indx = indx;
  this->_file.max_indx = max_indx;

  this->copyString(code_file, this->_file.code_file, VSP::CODE_FILE_SIZE);

  this->_file.to_read = to_read;

  this->copyString(file, this->_file.file, to_read/*VSP::FILE_SIZE*/);
  //memcpy(this->_file.file, file, to_read);// CHANGE
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


  this->copyString(reinterpret_cast<VSP::File *>(this->_body)->code_file, this->_file.code_file, VSP::CODE_FILE_SIZE);
  this->copyString(reinterpret_cast<VSP::File *>(this->_body)->file, this->_file.file, /*VSP::FILE_SIZE*/this->_file.to_read);
//  memcpy(this->_file.file, reinterpret_cast<VSP::File *>(this->_body)->file, this->_file.to_read); // CHANGE
}

void MessageFile::debug() {
	std::cout << "MessageFile::debug()" << std::endl;
	std::cout << "indx: " << this->_file.indx << " max_indx: " << this->_file.max_indx << " code_file: " << this->_file.code_file
		<< " to_read: " << this->_file.to_read << " file: " << this->_file.file << std::endl;
}
