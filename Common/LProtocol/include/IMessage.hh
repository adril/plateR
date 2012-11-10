#ifndef __IMESSAGE_HH__
#define __IMESSAGE_HH__

#include <Message.hh>
/**
 * \file IMessage.hh
 * \brief IMessage is the interface wich define a basic Message
 * \date 8 november 2012
 */


/**
 * \class IMessage
 * \brief IMessage is the interface wich define a basic Message
 */
class IMessage {
public:
  /**
   * \return return header content
   */
  virtual char *getHeader() const = 0;
  /**
   * \return return body content
   */
  virtual char *getBody() const = 0;
  /**
   * \return data to send
   */
  virtual char *getData() const = 0;
  /**
   * \return data type
   */
  virtual char getType() const = 0;
  /**
   * \return header length
   */
  virtual size_t getHeaderLength() const = 0;
  /**
   * \return body length
   */
  virtual size_t getBodyLength() const = 0;
  /**
   * \return all data length
   */
  virtual size_t getDataLength() const = 0;
  /**
   * \set error value
   */
  virtual void setError(char error) = 0;
  /**
   * \get error value
   */
  virtual char getError() const = 0;
  /**
   * \param type type of body data
   * \brief serialized header into a header data
   */
  virtual void encodeHeader(char type) = 0;
  /**
   * \brief serialized body into a body data
   */
  virtual void encodeBody() = 0;
  /**
   * \brief unserialized body
   */
  virtual void decodeBody() = 0;
  /**
   * \brief merge header and body data
   */
  virtual void encodeData() = 0;
  /**
   * \brief extract header data
   */
  virtual void decodeHeader() = 0;
  /**
   * \brief aff all value
   */

  virtual void debug() = 0;
};

#endif /* !__IMESSAGE_HH__ */

