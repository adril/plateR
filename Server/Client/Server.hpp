#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <map>
#include <time.h>
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include "Common.hpp"
#include "Client.hpp"


class Server
{
public :
	Server();
	~Server();

	void	run();
	void	stop();
private :
	std::list<Client*>	_clients;
	std::list<Client*>	_to_be_deleted;
	tcp::io_service			_io_service;
	tcp::acceptor			_acceptor;
	tcp::deadline_timer		_timer;
	bool				_isRunning;
	

	void startAccept();
	void handleAccept(Client *cl, tcp::error_code error);

	/* CLIENT MANAGEMENT */
	void deleteClients();
	void areClientsAlive();
};

#endif