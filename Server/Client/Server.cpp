#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <string>
#include "Server.hpp"

Server::Server() : 
	_acceptor(_io_service, tcp::endpoint(boost::asio::ip::tcp::v4(), DEFAULT_PORT)),
	_timer(_io_service)
{
	srand(time(NULL));
	_isRunning = true;
	_timer.expires_from_now(boost::posix_time::seconds(1));
	_timer.async_wait(boost::bind(&Server::areClientsAlive, this));
	startAccept();
}

Server::~Server()
{
	stop();
}

// Internal IO functions
void Server::run()
{
	_io_service.run();
}
void Server::stop()
{
	_io_service.stop();
	this->deleteClients();
	while (!_clients.empty())
	{
		delete _clients.front();
		_clients.pop_front();
	}
}

/* CLIENT MANAGEMENT */
void Server::areClientsAlive()
{
	/* CHECK EACH CLIENT */
	_timer.expires_from_now(boost::posix_time::seconds(5));
	_timer.async_wait(boost::bind(&Server::areClientsAlive, this));
	deleteClients();
}
void Server::addClient(Client *cl)
{
	cl->start();
	_clients.push_back(cl);
	std::cout << std::endl << "New client connected from " << cl->getIpAsString() << std::endl;
}
void Server::removeClient(Client *cl)
{
	std::cout << std::endl << "Client disconnected from " << cl->getIpAsString() << std::endl;
	_to_be_deleted.push_back(cl);
}
void Server::deleteClients()
{
	while (!_to_be_deleted.empty())
	{
		_clients.remove(_to_be_deleted.front());
		_to_be_deleted.pop_front();
	}
}

void Server::startAccept()
{
	Client	*new_client = new Client(_io_service);

	new_client->setDelegate(this);
	std::cout << "Server::start_accept" << std::endl;
	_acceptor.async_accept(new_client->getSocket(),
		boost::bind(&Server::handleAccept, this, new_client,
		boost::asio::placeholders::error));
}

void Server::handleAccept(Client *cl, tcp::error_code error) {
  if (!error)
	addClient(cl);
  else
    {
      std::cout << "ACCEPT FAIL" << std::endl;
	  disconectClient(cl);
    }
  startAccept();
}

void Server::disconectClient(void *client) {
	removeClient(reinterpret_cast<Client*>(client));
}