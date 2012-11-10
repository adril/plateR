#ifndef __MESSAGE_FILE_HH__
#define __MESSAGE_FILE_HH__

#include "Message.hh"

#ifdef WIN32
class CLASSINDLL_CLASS_DECL MessageFile : public Message {
#else
class MessageFile : public Message {
#endif
public:
	MessageFile(Message &other, char type, size_t indx, size_t max_indx, char *file_name, size_t to_read, char *file);
	MessageFile(Message &);
	virtual ~MessageFile();
	virtual void encodeBody();
	virtual void decodeBody();
	virtual void debug();
private:
	VSP::File _file;
};

#endif /* !__MESSAGE_FILE_HH__ */
