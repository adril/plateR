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
    typedef void (LProtocol::*protocolHandler)(Message &message);
  public:
    LProtocol();
    LProtocol(IProtocolDelegate *delegate);
    LProtocol(LProtocol const&);
    virtual ~LProtocol();
    virtual void receiveMessage(Message &message);
	IProtocolDelegate *_delegate;
  protected:
    /* main hash table */
    virtual void loginHandler(Message &message);
    virtual void loginResultHandler(Message &message);
    virtual void logOutHandler(Message &message);
    virtual void plateHandler(Message &message);
    virtual void fileHandler(Message &message);
    virtual void unknowMessageHandler(Message &message);
    /* misc */
    void initProtocolHandlerMap();
    void debug();
  protected:
    std::map<char, protocolHandler> _protocolHandlerMap;
  };

#endif /* !__LPROTOCOL_HH__ */
