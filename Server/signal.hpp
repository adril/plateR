#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <iostream>
#include <signal.h>
#include "server.hpp"

class Signal
{

private:

  static void	handlerSigint(int sig)
  {
    if (Server::exists())
      {
	Server::getInstance()->stop();
	std::cout << "[SIGINT: "<<sig<<"] Shutting down server..." << std::endl;
      }
  }

public:

  static void	catchSignals()
  {
    signal(SIGINT, handlerSigint);
  }
};

#endif
