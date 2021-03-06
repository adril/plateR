#ifndef __MESSAGELOGOUT_HH__
#define __MESSAGELOGOUT_HH__

#include "Message.hh"

#ifdef WIN32
class CLASSINDLL_CLASS_DECL MessageLogOut : public Message {
#else
class MessageLogOut : public Message {
#endif
public:
	MessageLogOut(Message &other, char type);
	MessageLogOut(Message &);
	virtual ~MessageLogOut();
	virtual void encodeBody();
	virtual void decodeBody();
	virtual void debug();;
public:
	VSP::LogOut _logOut;
};

#endif /* !__MESSAGELOGOUT_HH__ */
