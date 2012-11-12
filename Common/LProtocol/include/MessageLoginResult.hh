#ifndef __MESSAGELOGINRESULT_HH__
#define __MESSAGELOGINRESULT_HH__

#include "Message.hh"

#ifdef WIN32
class CLASSINDLL_CLASS_DECL MessageLoginResult : public Message {
#else
class MessageLoginResult : public Message {
#endif
public:
	MessageLoginResult(Message &, char type, char state);
	MessageLoginResult(Message &);
	virtual ~MessageLoginResult();
	virtual void encodeBody();
	virtual void decodeBody();
	virtual void debug();;
public:
	VSP::LoginResult _loginResult;
};

#endif /* !__MESSAGELOGINRESULT_HH__ */
