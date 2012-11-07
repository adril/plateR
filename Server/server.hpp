/**
 ** Babel Server
 ** SERVER.HPP
 **
 **/

#ifndef BABEL_SERVER
#define BABEL_SERVER

#include <map>
#include <time.h>
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include "boost/asio/deadline_timer.hpp"
#include "common.hpp"
#include "client.hpp"


#define DISCONNECTION_DELAY_S 120
#define INACTIVITY_DELAY_S DISCONNECTION_DELAY_S/4

class Client;
class Server
{
  typedef void		(Server::*callback)(Client *, void *data);
  typedef callback	callbackMap[REQUESTCOUNT];

  enum		callState
    {
      WAITING_FOR_ENDPOINT,
      IN_CALL
    };

  struct	conversation
  {
    Client		*caller;
    Client		*recv;
    callState		state;
    conversation(Client *from, Client *to, callState st = WAITING_FOR_ENDPOINT)
      : caller(from), recv(to), state(st) {}
  };


public :
  static Server		*getInstance();
  static void		destroy();
  static void		setPort(unsigned short p);
  static bool		exists();
  void	addToDeleteList(Client *cl);
  void	handleRequest(Client *cl, tcp::Request, void *data);
  void	run();
  void	stop();

  // Debug
  std::fstream	log;

private :
  ////// SINGLETON //////
  Server(unsigned short port);
  ~Server();
  static Server			*instance;
  static tcp::io_service	io_service;
  static unsigned short		port;

private :
  void			startAccept();
  void			handleAccept(Client *cl, tcp::error_code error);
  void			notifyClientSuppression(Client *cl);
  Token			genToken();
  void			areClientsAlive();
  Client		*getClientByLogin(Login);
  conversation		*getConversationByReciever(Client *);
  conversation		*getConversationByClient(Client *);
  void			removeFromConversations(Client *);
  void			deleteClients();
  bool			isInList(std::list<Client*> &, Client *);

  /*
   * Protocol handling functions
   */
  void			_handleAuth(Client *, void *data);
  void			_sendUserList(Client *, void *data);
  void			_sendUserInfo(Client *, void *data);
  void			_chgUserState(Client *, void *data);
  void			_handleCall(Client *, void *data);
  void			_handleDisconnect(Client *, void *data);
  void			_handlePing(Client *, void *data);
  void			_handlePong(Client *, void *data);
  void			_callEnd(Client *, void *data);
  void			_forwardCallEndpoint(Client *, void *data);
  
private :

  callbackMap			_callback_map;


  std::list<Client*>		_clients;
  std::list<Client*>		_to_be_deleted;
  std::list<Client*>		_deleted;
  std::list<conversation>	_conversations;


  tcp::acceptor			_acceptor;
  bool				_isRunning;
  boost::asio::deadline_timer			_timer;
};

#endif
