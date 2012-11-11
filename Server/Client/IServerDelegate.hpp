#ifndef __ISERVERDELEGATE_HH__
#define __ISERVERDELEGATE_HH__

class IServerDelegate {
public:
  virtual void disconectClient(void *client) = 0;
};

#endif /* !__ISERVERDELEGATE_HH__ */