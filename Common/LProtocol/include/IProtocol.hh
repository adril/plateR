#ifndef __IPROTOCOL_HH__
#define __IPROTOCOL_HH__

#include <deque>
#include "Message.hh"

class IProtocol {
public:
	virtual void receiveMessage(Message &message) = 0;

protected:
	/* Misc func */

	virtual void loginHandler(Message &message) = 0;
	virtual void loginResultHandler(Message &message) = 0;
	virtual void logOutHandler(Message &message) = 0;
	virtual void plateHandler(Message &message) = 0;
	virtual void fileHandler(Message &message) = 0;
	virtual void unknowMessageHandler(Message &message) = 0;

public:
	virtual void debug() = 0;

};

#endif /* !__IPROTOCOL_HH__ */
