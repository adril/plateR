#ifndef __ISERVERDELEGATE_HH__
#define __ISERVERDELEGATE_HH__

class IServerDelegate {
public:
  virtual void disconectClient(void *client) = 0;
  virtual std::string recognizePlate(void *client, std::string code_file) = 0;
  virtual std::string getPlateValue(void *client, std::string code_file) = 0;
};

#endif /* !__ISERVERDELEGATE_HH__ */