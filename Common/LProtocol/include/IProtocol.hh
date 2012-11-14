#ifndef __IPROTOCOL_HH__
#define __IPROTOCOL_HH__

#include <deque>
#include "Message.hh"


class IProtocol {
public:
	//INFO: return value define if we can continue to read after the receiveMessage
	virtual bool receiveMessage(Message &message) = 0;

protected:
	/* Misc func */

	virtual bool loginHandler(Message &message) = 0;
	virtual bool loginResultHandler(Message &message) = 0;
	virtual bool logOutHandler(Message &message) = 0;
	virtual bool plateHandler(Message &message) = 0;
	virtual bool fileHandler(Message &message) = 0;
	virtual bool unknowMessageHandler(Message &message) = 0;

public:
	virtual void debug() = 0;

};

#endif /* !__IPROTOCOL_HH__ */
