#include "LProtocol.hh"

LProtocol::LProtocol() {
	std::cout  << "LProtocol::LProtocol()" << std::endl;
	this->initProtocolHandlerMap();
	this->_countUnknowMessage = 0;
}

LProtocol::LProtocol(IProtocolDelegate *delegate) {
	std::cout  << "LProtocol::LProtocol(IProtocolDelegate *delegate)" << std::endl;

	this->initProtocolHandlerMap();
	this->_delegate = delegate;
	this->_countUnknowMessage = 0;
}

LProtocol::LProtocol(LProtocol const & other) {
	std::cout  << "LProtocol::LProtocol(LProtocol const & other)" << std::endl;
	this->_protocolHandlerMap = other._protocolHandlerMap;
	this->_countUnknowMessage = other._countUnknowMessage;
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
bool LProtocol::receiveMessage(Message &message) {
	std::cout << "LProtocol::receiveMessage(Message &message)"<< std::endl;
	bool result = false;

	if (message.getHeader() && message.getBody()) {
		std::cout << "[LProtocol] type: " << (int)message.getType() << std::endl;
		if (IS_IN_RANGE(message.getType(), VSP::LOGIN, VSP::FILE))
			result = (this->*_protocolHandlerMap[(int)message.getType()])(message);
		else
			result = this->unknowMessageHandler(message);
	}
	return result;
}

//second

bool LProtocol::loginHandler(Message &message) {
	if (this->_delegate) {
		MessageLogin *customMessage = new MessageLogin(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->loginHandler(*customMessage);
	}
	return true;
}

bool LProtocol::loginResultHandler(Message &message) {
	if (this->_delegate) {
		MessageLoginResult *customMessage = new MessageLoginResult(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->loginResultHandler(*customMessage);
	}
	return true;
}

bool LProtocol::logOutHandler(Message &message) {
	if (this->_delegate) {
		MessageLogOut *customMessage = new MessageLogOut(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->logOutHandler(*customMessage);
	}
	return false;
}

bool LProtocol::plateHandler(Message &message) {
	if (this->_delegate) {
		MessagePlate *customMessage = new MessagePlate(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->plateHandler(*customMessage);
	}
	return true;
}

bool LProtocol::fileHandler(Message &message) {
	if (this->_delegate) {
		MessageFile *customMessage = new MessageFile(message);
		customMessage->decodeBody();
		customMessage->debug();

		this->_delegate->fileHandler(*customMessage);
	}
	return true;
}

bool LProtocol::unknowMessageHandler(Message &message) {
	std::cout << "[LProtocol] unknowMessage: " << this->_countUnknowMessage++ << std::endl;
	if (this->_delegate)
		return this->_delegate->unknowMessageHandler(message);
	return true;
}

void LProtocol::debug() {
	//std::cout << "LProtocol::debug()" << std::endl;
}

#ifdef _WIN32
extern "C" __declspec(dllexport) LProtocol* __cdecl getNewLProtocol()
#elif __unix__
extern "C" LProtocol* getNewLProtocol()
#endif
{
	return (new LProtocol());
}
