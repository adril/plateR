#ifndef __MESSAGEPLATE_HH__
#define __MESSAGEPLATE_HH__

#include "Message.hh"

#ifdef WIN32
class CLASSINDLL_CLASS_DECL MessagePlate : public Message {
#else
class MessagePlate : public Message {
#endif
public:
	MessagePlate(Message &other, char type, char *codePlate, char *name, char *codeFile, char state);
	MessagePlate(Message &);
	virtual ~MessagePlate();
	virtual void encodeBody();
	virtual void decodeBody();
	virtual void debug();
public:
	VSP::Plate _plate;
};

#endif /* !__MESSAGEPLATE_HH__ */
