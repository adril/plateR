#include "../../include/LProtocol.hh"

LProtocol::LProtocol() {
  std::cout  << "[LProtocol]" << std::endl;
  this->initProtocolHandlerMap();
}

LProtocol::LProtocol(IProtocolDelegate *delegate) {
  std::cout  << "[LProtocol]" << std::endl;

  this->initProtocolHandlerMap();
  this->_delegate = delegate;
}

LProtocol::LProtocol(LProtocol const & other) {
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
  std::cout << "[LProtocol] receiveMessage"<< std::endl;
    if (message.getHeader() && message.getBody()) {
      std::cout << "[LProtocol] type: " << (int)message.getType() << std::endl;
      if (IS_IN_RANGE/*(((VSP::Connection *)message.getHeader())->type*/(message.getType(), VSP::LOGIN, VSP::FILE))
	(this->*_protocolHandlerMap[message.getType()/*((VSP::Header *)message.getHeader())->type*/])(message);
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
  this->_delegate->loginHandler(message);
}

void LProtocol::loginResultHandler(Message &message) {
  this->_delegate->loginResultHandler(message);
}

void LProtocol::logOutHandler(Message &message) {
  this->_delegate->loginHandler(message);
}

void LProtocol::plateHandler(Message &message) {
  this->_delegate->loginHandler(message);
}

void LProtocol::fileHandler(Message &message) {
  this->_delegate->loginHandler(message);
}

void LProtocol::unknowMessageHandler(Message &message) {
  (void)message;
  std::cout << "[LProtocol] unknowMessage" << std::endl;
  this->_delegate->unknowMessageHandler(message);
}

void LProtocol::debug() {
  std::cout << "[LProtocol] aff Hello World" << std::endl;
  /*
  std::cut << "[LProtocol] aff hello" << std::endl;
  {
    Message &message = new Message();
    message.encodeHeader(RVSP::CMD_CONNECTION, "adril");
    message.decodeHeader();
    message.getHeader();
    //read on message.getBody(); size of message.getBodyLength();
    {
      MessageConnection *messageConnection = new MessageConnection(*message, RVSP::CONNECT, RVSP::ERROR_NOTHING);
      messageConnection->encodeBody();
      RVSP::Connection *tmpConnection = reinterpret_cast<RVSP::Connection *>(messageConnection->getBody());
      std::cout << "[Messageconnection]" << messageConnection->getLogin() << " bodyLength_: " << messageConnection->getBodyLength() << std::endl;
      std::cout << "[Messageconnection] getBody() | command: " << (int)tmpConnection->command << std::endl;

      messageConnection->encodeData();
      messageConnection->sendList_.push_back(User("login", 0, 5));
      char const *msgTmp = messageConnection->getData() + messageConnection->getHeaderLength();
      RVSP::Connection *dataTmp = (RVSP::Connection*)(msgTmp);
      std::cout << "[Message] getData()"<< (int)dataTmp->command << std::endl;
      this->messageList_.push_back(messageConnection);
    }
  }
  */
}

#ifdef _WIN32
extern "C" __declspec(dllexport) LProtocol* __cdecl getNewLProtocol()
#elif __unix__
extern "C" LProtocol* getNewLProtocol()
#endif
{
    return (new LProtocol());
}
