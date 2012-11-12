#ifndef __IPROTOCOLDELEGATE_HH__
#define __IPROTOCOLDELEGATE_HH__

class IProtocolDelegate {
public:
  virtual void loginHandler(MessageLogin &message) = 0;
  virtual void loginResultHandler(MessageLoginResult &message) = 0;
  virtual void logOutHandler(MessageLogOut &message) = 0;
  virtual void plateHandler(MessagePlate &message) = 0;
  virtual void fileHandler(MessageFile &message) = 0;
  virtual void unknowMessageHandler(Message &message) = 0;
};

#endif /* !__IPROTOCOLDELEGATE_HH__ */
