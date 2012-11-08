#ifndef __IPROTOCOLDELEGATE_HH__
#define __IPROTOCOLDELEGATE_HH__

class IProtocolDelegate {
public:
  virtual void loginHandler(Message &message) = 0;
  virtual void loginResultHandler(Message &message) = 0;
  virtual void logOutHandler(Message &message) = 0;
  virtual void plateHandler(Message &message) = 0;
  virtual void fileHandler(Message &message) = 0;
  virtual void unknowMessageHandler(Message &message) = 0;
};

#endif /* !__IPROTOCOLDELEGATE_HH__ */
