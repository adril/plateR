#ifndef __MESSAGELOGIN_HH__
#define __MESSAGELOGIN_HH__

#include "Message.hh"

#ifdef WIN32
class CLASSINDLL_CLASS_DECL MessageLogin : public Message {
#else
class MessageLogin : public Message {
#endif
public:
	MessageLogin(Message &other, char *login, char *password);
	MessageLogin(Message &);
	virtual ~MessageLogin();
	virtual void encodeBody();
	virtual void decodeBody();
	virtual void debug();
public:
	VSP::Login _login;
};

#endif /* !__MESSAGELOGIN_HH__ */
