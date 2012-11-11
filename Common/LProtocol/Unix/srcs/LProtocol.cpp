#include "LProtocol.hh"

LProtocol::LProtocol() {
	std::cout  << "LProtocol::LProtocol()" << std::endl;
	this->initProtocolHandlerMap();
}

LProtocol::LProtocol(IProtocolDelegate *delegate) {
	std::cout  << "LProtocol::LProtocol(IProtocolDelegate *delegate)" << std::endl;

	this->initProtocolHandlerMap();
	this->_delegate = delegate;
}

LProtocol::LProtocol(LProtocol const & other) {
	std::cout  << "LProtocol::LProtocol(LProtocol const & other)" << std::endl;
	this->_protocolHandlerMap = other._protocolHandlerMap;
}

LProtocol::~LProtocol() {}

void LProtocol::initProtocolHandlerMap() {
	/* MAIN MAP */
	this->_protocolHandlerMap[VSP::LOGIN] = &LProtocol::loginHandler;
	this->_protocolHandlerMap[VSP::LOGIN_RESULT] = &LProtocol::loginResultHandler;
	this->_protocolHandlerMap[VSP::LOGOUT] = &LProtocol::logOutHandler;
	this->_protocolHandlerMap[VSP::PLATE] = &LProtocol::plateHandler;
	this->_protocolHandlerMap[VSP::FILE] = &LProtocol::fileHandler;
}

//main
void LProtocol::receiveMessage(Message &message) {
	std::cout << "LProtocol::receiveMessage(Message &message)"<< std::endl;
	if (message.getHeader() && message.getBody()) {
		std::cout << "[LProtocol] type: " << (int)message.getType() << std::endl;
		if (IS_IN_RANGE(message.getType(), VSP::LOGIN, VSP::FILE))
			(this->*_protocolHandlerMap[(int)message.getType()])(message);
		else
			this->unknowMessageHandler(message);
	}
}
/*
std::cout << "[LProtocol]connectionP_receive message of type Connetion with body command: "<< (int)((RVSP::Connection *)message.getBody())->command << std::endl;
std::cout << "((RVSP::Header *)message.getHeader())->size_struct = " << ((RVSP::Header *)message.getHeader())->size_struct << std::endl;
*/

//second

void LProtocol::loginHandler(Message &message) {
	if (this->_delegate) {
		MessageLogin *customMessage = new MessageLogin(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->loginHandler(*customMessage);
	}
}

void LProtocol::loginResultHandler(Message &message) {
	if (this->_delegate) {
		MessageLoginResult *customMessage = new MessageLoginResult(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->loginResultHandler(*customMessage);
	}
}

void LProtocol::logOutHandler(Message &message) {
	if (this->_delegate) {
		MessageLogOut *customMessage = new MessageLogOut(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->logOutHandler(message);
	}
}

void LProtocol::plateHandler(Message &message) {
	if (this->_delegate) {
		MessagePlate *customMessage = new MessagePlate(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->plateHandler(message);
	}
}

void LProtocol::fileHandler(Message &message) {
	if (this->_delegate) {
		MessageFile *customMessage = new MessageFile(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->fileHandler(message);
	}
}

void LProtocol::unknowMessageHandler(Message &message) {
	std::cout << "[LProtocol] unknowMessage" << std::endl;
	if (this->_delegate)
		this->_delegate->unknowMessageHandler(message);
}

void LProtocol::debug() {
	std::cout << "LProtocol::debug()" << std::endl;
}

#ifdef _WIN32
extern "C" __declspec(dllexport) LProtocol* __cdecl getNewLProtocol()
#elif __unix__
extern "C" LProtocol* getNewLProtocol()
#endif
{
	return (new LProtocol());
}
