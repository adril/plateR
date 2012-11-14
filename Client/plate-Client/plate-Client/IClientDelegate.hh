#ifndef __ICLIENTDELEGATE_HH__
#define __ICLIENTDELEGATE_HH__

class IClientDelegate {
public:
  virtual void loginResultHandler(bool result) = 0;
  virtual void logOutHandler() = 0;
  virtual void plateLiveHandler(std::string codeFile) = 0;
  virtual void plateRecordedHandler(std::string codeFile) = 0;
  virtual void fileHandler(std::string codeFile) = 0;
  virtual bool unknowMessageHandler() = 0;
};

#endif /* !__ICLIENTDELEGATE_HH__ */
