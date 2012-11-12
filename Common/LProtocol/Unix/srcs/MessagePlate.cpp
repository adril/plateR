#include "../include/MessagePlate.hh"

MessagePlate::MessagePlate(Message &other, char type, char *codePlate, char *name, char *codeFile, char state)
	: Message(other) {
		this->_plate.type = type;

		this->copyString(codePlate, this->_plate.code_plate, VSP::CODE_PLATE_SIZE);
		this->copyString(name, this->_plate.name, VSP::NAME_SIZE);
		this->copyString(codeFile, this->_plate.code_file, VSP::CODE_FILE_SIZE);

		this->_plate.state = state;
}

MessagePlate::MessagePlate(Message &other)
	: Message(other) {
		std::cout  << "[MessagePlate] ------ (Message const& other)" << std::endl;
}

MessagePlate::~MessagePlate() {}

void MessagePlate::encodeBody() {
	std::memcpy(this->_body, &this->_plate, sizeof(this->_plate));
}

void MessagePlate::decodeBody() {
	this->_plate.type = reinterpret_cast<VSP::Plate *>(this->_body)->type;

	this->copyString(reinterpret_cast<VSP::Plate *>(this->_body)->code_plate, this->_plate.code_plate, VSP::CODE_PLATE_SIZE);
	this->copyString(reinterpret_cast<VSP::Plate *>(this->_body)->name, this->_plate.name, VSP::NAME_SIZE);
	this->copyString(reinterpret_cast<VSP::Plate *>(this->_body)->code_file, this->_plate.code_file, VSP::CODE_FILE_SIZE);

	this->_plate.state = reinterpret_cast<VSP::Plate *>(this->_body)->state;
}

void MessagePlate::debug() {
	std::cout << "MessageLogOut::debug()" << std::endl;
	std::cout << "code_plate: " << this->_plate.code_plate << " name: " << this->_plate.name << " code_file: "<< this->_plate.code_file
		<< " state: " << (int)this->_plate.state << std::endl;
}