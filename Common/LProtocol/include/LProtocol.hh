#ifndef __LPROTOCOL_HH__
#define __LPROTOCOL_HH__

#include <iostream>
#include <map>

#include "IProtocol.hh"
#include "Message.hh"

#include "MessageLogin.hh"
#include "MessageLoginResult.hh"
#include "MessageLogOut.hh"
#include "MessagePlate.hh"
#include "MessageFile.hh"

#include "IProtocolDelegate.hh"

#ifdef WIN32
class CLASSINDLL_CLASS_DECL LProtocol : public IProtocol {
#else
  class LProtocol : public IProtocol {
#endif
    typedef bool (LProtocol::*protocolHandler)(Message &message);
  public:
    LProtocol();
    LProtocol(IProtocolDelegate *delegate);
    LProtocol(LProtocol const&);
    virtual ~LProtocol();
    virtual bool receiveMessage(Message &message);
	IProtocolDelegate *_delegate;
  protected:
    /* main hash table */
    virtual bool loginHandler(Message &message);
    virtual bool loginResultHandler(Message &message);
    virtual bool logOutHandler(Message &message);
    virtual bool plateHandler(Message &message);
    virtual bool fileHandler(Message &message);
    virtual bool unknowMessageHandler(Message &message);
    /* misc */
    void initProtocolHandlerMap();
    void debug();
  protected:
    std::map<char, protocolHandler> _protocolHandlerMap;
	size_t _countUnknowMessage;
  };

#endif /* !__LPROTOCOL_HH__ */
