#include <time.h>
#include "signal.hpp"
#include "server.hpp"




int	main(int argc,  char **argv)
{

  std::cout << "-->ui" << sizeof(userInfo) << " req :" << sizeof(tcp::Request)<<  std::endl;
  try
    {
      if (argc >= 2)
	Server::setPort(atoi(argv[1]));
      
      Server	*s = Server::getInstance();

      Signal::catchSignals();

      s->run();		// Main Loop
      s->destroy();
    }
  catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
      Server	*s = Server::getInstance();
      s->run();		// Main Loop
      s->destroy();
    }

  return 0;
}
