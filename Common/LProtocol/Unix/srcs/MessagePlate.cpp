#include "../include/MessagePlate.hh"

MessagePlate::MessagePlate(Message &other, char type, char *codePlate, char *name, char *codeImage, char state)
  : Message(other) {
  this->_plate.type = type;

  //  std::memset(this->_plate.name, '\0', VSP::NAME_SIZE);
  //  std::strcpy(this->_plate.name, name);

  this->copyString(codePlate, this->_plate.code_plate, VSP::CODE_PLATE_SIZE);
  this->copyString(name, this->_plate.name, VSP::NAME_SIZE);
  this->copyString(codeImage, this->_plate.code_image, VSP::CODE_IMAGE_SIZE);

  this->_plate.state = state;
}

MessagePlate::MessagePlate(Message &other) {
  std::cout  << "[MessagePlate] ------ (Message const& other)" << std::endl;

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

MessagePlate::~MessagePlate() {}

void MessagePlate::encodeBody() {
  std::memcpy(this->_body, &this->_plate, sizeof(this->_plate));
}

void MessagePlate::encodeBodyWithError(char error) {
  this->_error = error;
  encodeBody();
}

void MessagePlate::decodeBody() {
  this->_plate.type = reinterpret_cast<VSP::Plate *>(this->_body)->type;
  //  std::memset(this->_plate.name, '\0', VSP::NAME_SIZE);
  //  std::strcpy(this->_plate.name, reinterpret_cast<VSP::Member *>(this->_body)->name);


  std::strcpy(this->_plate.code_plate, reinterpret_cast<VSP::Plate *>(this->_body)->code_plate);

  //  this->_plate.code_plate = reinterpret_cast<VSP::Plate *>(this->_body)->code_plate;
//  this->_plate.name = reinterpret_cast<VSP::Plate *>(this->_body)->name;
  // this->_plate.code_image = reinterpret_cast<VSP::Plate *>(this->_body)->code_image;
  this->_plate.state = reinterpret_cast<VSP::Plate *>(this->_body)->state;
}
