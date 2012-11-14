#ifndef __IPROTOCOLDELEGATE_HH__
#define __IPROTOCOLDELEGATE_HH__

class IProtocolDelegate {
public:
  virtual bool loginHandler(MessageLogin &message) = 0;
  virtual bool loginResultHandler(MessageLoginResult &message) = 0;
  virtual bool logOutHandler(MessageLogOut &message) = 0;
  virtual bool plateHandler(MessagePlate &message) = 0;
  virtual bool fileHandler(MessageFile &message) = 0;
  virtual bool unknowMessageHandler(Message &message) = 0;
};

#endif /* !__IPROTOCOLDELEGATE_HH__ */
