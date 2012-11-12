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

	/* copy of data */

	this->_header = other.getHeader();
	this->_body = other.getBody();
	///this->_data = other.getData();

	/* copy of header infos */
	this->_type = other.getType();
	this->_bodyLength = other.getBodyLength();

	this->_error = other.getError();
}

Message::~Message() {
	std::cout << "Message::~Message"<< std::endl;
	clean();
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
	return this->getHeaderLength() + this->getBodyLength();
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

	VSP::Header header;

	header.type = type;
	header.body_size = this->_dataTypeSizeMap[type];
	header.error = this->_error;

	std::memset(this->_header, '\0', this->getHeaderLength());
	std::memcpy(this->_header, &header, this->getHeaderLength());
}

void Message::encodeHeader() {
	std::cout << "[Message] encodeHeader" << std::endl;
	this->encodeHeader(this->_type);
}

void Message::encodeBody() {
	std::cout << "[Message] encodeBody -> neverCall"<< std::endl;
	//TODO: to impl in inherit obj [OK]
}

void Message::decodeBody() {
	std::cout << "[Message] decodeBody -> neverCall"<< std::endl;
}

void Message::encodeData() {
	int dataLength = this->getHeaderLength() + this->getBodyLength();

	this->_data = new char[dataLength];
	std::memset(this->_data, '\0', dataLength);

	std::memcpy(this->_data, this->_header, this->getHeaderLength());
	std::memcpy(this->_data + this->getHeaderLength(), this->_body, this->getBodyLength());
}

void Message::decodeHeader() {
	this->_type = reinterpret_cast<VSP::Header *>(this->_header)->type;
	this->_bodyLength = this->_dataTypeSizeMap[(int)this->_type];
	this->_error = reinterpret_cast<VSP::Header *>(this->_header)->error;

	//init body
	this->_body = new char[this->getBodyLength()];
	std::memset(this->_body, '\0', this->getBodyLength());
}

void Message::clean() {
	//delete[] this->_body;
	//delete[] this->_header;
	//delete[] this->_data;
}

//INFO: tools

void Message::copyString(char *src, char *dest, size_t size) {
	std::memset(dest, '\0', size);
	std::strcpy(dest, src);
}

void Message::debug() {
	std::cout << "[Message] debug()" << std::endl;
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
