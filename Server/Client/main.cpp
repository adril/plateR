#include <time.h>
#include "server.hpp"

int	main(int argc,  char **argv)
{

  std::cout << "--> Server Listening on port " << DEFAULT_PORT << " <--" << std::endl;
	Server	*s = new Server();
	s->run(); // Main Loop
	delete s;
  return 0;
}