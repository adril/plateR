#ifndef __MESSAGE_HH__
#define __MESSAGE_HH__

#define _CRT_SECURE_NO_DEPRECATE 1
#include <iostream>
#include <map>
#include <deque>

#include "IMessage.hh"
#include "VSP.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

#ifdef WIN32
#include "../../ClassInDll.h"

class CLASSINDLL_CLASS_DECL Message : public IMessage {
#else
class  Message : public IMessage {
#endif
public:
	Message();
	Message(Message const& other);
	virtual ~Message();
	void initDataTypeSizeMap();
	virtual char *getHeader() const;
	virtual char *getBody() const;
	virtual char *getData()const;
	size_t getHeaderLength() const;
	size_t getBodyLength() const;
	size_t getDataLength() const;
	char getType() const;
	virtual void setError(char error);
	char getError() const;
	virtual void encodeHeader(char type);
	virtual void encodeHeader();
	virtual void encodeData();
	virtual void decodeHeader(void);
	virtual void encodeBody(void);
	virtual void decodeBody(void);
	bool isValidHeader();
	void clean();;
	void copyString(char *src, char *dest, size_t size);
	virtual void debug();
protected:
	char *_header;
	char *_body;
	char *_data;
	size_t _bodyLength;
	char _type;
	char _error;
	std::map<char, size_t> _dataTypeSizeMap;
};

#endif /* !__MESSAGE_HH__ */
