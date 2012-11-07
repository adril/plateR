#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <string>
#include "server.hpp"

////////////////////////
///// CONSTRUCTION /////
////////////////////////

// Public Statics
// Private Statics
tcp::io_service Server::io_service;
unsigned short Server::port = DEFAULT_PORT;
Server*	Server::instance = NULL;

//static
Server::Server(unsigned short port)
  :_acceptor(io_service, tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
   _timer(io_service)
{
  /*
   * Init
   */
  srand(time(NULL));
  _isRunning = true;
  _timer.expires_from_now(boost::posix_time::seconds(1));
  _timer.async_wait(boost::bind(&Server::areClientsAlive, this));
  memset(_callback_map, 0, sizeof(_callback_map));
  log.open("./babelserv.log", std::fstream::in | std::fstream::out | std::fstream::trunc);
  log << "Opening Stream:" << std::endl;
  /*
   *  Callback association
   */
  _callback_map[tcp::R_AUTH] = &Server::_handleAuth;
  _callback_map[tcp::R_GETUSERLIST] = &Server::_sendUserList;
  _callback_map[tcp::R_GETUSERINFO] = &Server::_sendUserInfo;
  _callback_map[tcp::R_SETSTATE] = &Server::_chgUserState;
  _callback_map[tcp::R_CALLUSER] = &Server::_handleCall;
  _callback_map[tcp::R_DISCONNECT] = &Server::_handleDisconnect;
  _callback_map[tcp::R_CALLENDED] = &Server::_callEnd;
  _callback_map[tcp::R_PING] = &Server::_handlePing;
  _callback_map[tcp::R_PONG] = &Server::_handlePong;
  _callback_map[tcp::R_CALLENDPOINT] = &Server::_forwardCallEndpoint;
  /*
   * Start
   */
  startAccept();
}

//static
Server::~Server()
{
  std::cout << "[DESTROYING SERVER]" << std::endl;
  log << "Closing:" << std::endl;
  log.close();
  while (!_clients.empty())
    {
      delete _clients.front();
      _clients.pop_front();
    }
}

//static
Server *Server::getInstance()
{
  if (instance == NULL)
    instance = new Server(port);
  return instance;
}

//static
void	Server::setPort(unsigned short p)
{
  port = p;
}

///////////////////////
/////  MAIN LOOP  /////
///////////////////////

void	Server::run()
{
  io_service.run();
}

void	Server::areClientsAlive()
{
  std::list<Client*>::iterator it = _clients.begin();
  std::list<Client*>::iterator end = _clients.end();

  std::cout << ".";
  std::cout.flush();

  while (it != end)
    {
      (*it)->setTimestamp((*it)->getTimestamp() + 1);
      if ((*it)->getTimestamp() >= DISCONNECTION_DELAY_S)
	{
	  (*it)->pushAnswer(tcp::R_TIMEOUT, NULL, 0);
	  (*it)->getUserInfo()->token = -1;
	  _to_be_deleted.push_back(*it);
	}
      else if ((*it)->getTimestamp() >= INACTIVITY_DELAY_S)
	{
	  std::cout << "[AreClientsAlive] Pinging..." << std::endl;
	  (*it)->pushAnswer(tcp::R_PING, NULL, 0);
	}
      ++it;
    }
  deleteClients();
  while (!_deleted.empty())
    {
      _deleted.pop_front();
    }
  _timer.expires_from_now(boost::posix_time::seconds(1));
  _timer.async_wait(boost::bind(&Server::areClientsAlive, this));
}

Server::conversation	*Server::getConversationByReciever(Client *rc)
{
  std::list<conversation>::iterator it = _conversations.begin();

  while (it != _conversations.end())
    {
      if ((*it).recv == rc)
	return &(*it);
      ++it;
    }
  return NULL;
}

Server::conversation	*Server::getConversationByClient(Client *cl)
{
  std::list<conversation>::iterator it = _conversations.begin();

  while (it != _conversations.end())
    {
      if ((*it).recv == cl || (*it).caller == cl)
	return &(*it);
      ++it;
    }
  return NULL;
}

void			Server::removeFromConversations(Client *cl)
{
  std::list<conversation>::iterator it = _conversations.begin();

  while (it != _conversations.end())
    {
      if ((*it).recv == cl || (*it).caller == cl)
	{
	  _conversations.erase(it);
	  return ;
	}
      ++it;
    }
}

Client	*Server::getClientByLogin(Login l)
{
  std::list<Client*>::iterator it = _clients.begin();
  std::string tmp = l;

  while (it != _clients.end())
    {
      if (isInList(_to_be_deleted, *it) == false
	  && tmp == (*it)->getUserInfo()->login)
	return *it;
      ++it;
    }
  std::cout << "User not found : " << tmp << std::endl;
  return NULL;
}

void	Server::stop()
{
  io_service.stop();
}

//static
void	Server::destroy()
{
  if (instance != NULL)
    {
      delete instance;
      instance = NULL;
    }
}

//static
bool	Server::exists()
{
  if (instance != NULL)
    return true;
  return false;
}

void	Server::deleteClients()
{
  if (_to_be_deleted.size())
    {
      std::cout << "\n=========[DELETE CLIENTS]=========" << std::endl;
      _to_be_deleted.sort();
      _to_be_deleted.unique();
      for (std::list<Client*>::iterator it = _to_be_deleted.begin();
	   it != _to_be_deleted.end(); ++it)
	{
	  notifyClientSuppression(*it);
	}
      while (!_to_be_deleted.empty())
	{
	  if (_to_be_deleted.front() && isInList(_deleted, _to_be_deleted.front()) == false)
	    {
	      std::cout << "Deleting client : "
			<< _to_be_deleted.front()->getUserInfo()->login
			<< std::endl;
	      log << "[DELETE CLIENT] "
		   << _to_be_deleted.front()->getUserInfo()->login
		   << std::endl;
	      _clients.remove(_to_be_deleted.front());
	      delete _to_be_deleted.front();
	    }
	  else
	    {
	      std::cout << "Oh no ! This should not have happened" << std::endl;
	      log << "[ERROR] Attempting to delete a non-existing client" << std::endl;
	    }
	  _to_be_deleted.pop_front();
	}
      std::cout << "==================================" << std::endl;
    }
}

void		Server::notifyClientSuppression(Client *cl)
{
  std::list<Client*>::iterator		it;
  conversation				*cv;

  if ((cv = getConversationByClient(cl)) != NULL)
    {
      Client *correspondant;

      if (cv->caller == cl)
	correspondant = cv->recv;
      else
	correspondant = cv->caller;
      if (std::find(_to_be_deleted.begin(),
		       _to_be_deleted.end(), correspondant) == _to_be_deleted.end())
	correspondant->pushAnswer(tcp::R_CALLENDED, cl->getUserInfo()->login,
				  sizeof(Login));
      removeFromConversations(cl);
    }
  for (it = _clients.begin(); it != _clients.end(); ++it)
    {
      if ((*it) != cl
	  && std::find(_to_be_deleted.begin(),
		       _to_be_deleted.end(), *it) == _to_be_deleted.end())
	(*it)->pushAnswer(tcp::R_DISCONNECTED, cl->getUserInfo()->login, sizeof(Login));
    }
}

void	Server::addToDeleteList(Client *cl)
{
  _to_be_deleted.push_back(cl);
}

bool	Server::isInList(std::list<Client*> & l, Client *cl)
{
  return (std::find(l.begin(), l.end(), cl) != l.end());
}

//////////////////
///// ACCEPT /////
//////////////////

void		Server::startAccept()
{
  Client	*new_client = new Client(io_service, this);

  std::cout << "Server::start_accept" << std::endl;
  _acceptor.async_accept(new_client->getSocket(),
			 boost::bind(&Server::handleAccept, this, new_client,
			 boost::asio::placeholders::error));
}

void		Server::handleAccept(Client *cl, tcp::error_code error)
{
  if (!error)
    {
      cl->start();
      _clients.push_back(cl);
      std::cout << std::endl << "New client connected from " << cl->getIpAsString() << std::endl;
    }
  else
    {
      std::cout << "ACCEPT FAIL" << std::endl;
      addToDeleteList(cl);
    }
  startAccept();
}

////////////////////
///// PROTOCOL /////
////////////////////

void		Server::handleRequest(Client *cl, tcp::Request r, void *data)
{
  std::cout << "Recieved tcp request : " << r << std::endl;

  if (isInList(_to_be_deleted, cl))
    return ;

  if (r >= 0 && r < REQUESTCOUNT)
    {
      if (_callback_map[r] == NULL ||
	  ((cl->getUserInfo()->token <= 0 && r != tcp::R_AUTH)))
	{
	  cl->pushError(tcp::error::E_DENIED);
	}
      else
	(this->*_callback_map[r])(cl, data);
    }
  else
    {
      std::cout << "@@@@@ INVALID REQUEST : " << r << std::endl;
      cl->pushError(tcp::error::E_INVCMD);
    }
  cl->setTimestamp(0);
}

//Callbacks :

/**
 ** _handleAuth (answers to R_AUTH with R_SETTOKEN)
 ** Saves the user's info
 ** A new token is assigned and sent to the user
 **/
void		Server::_handleAuth(Client *cl, void *data)
{
  bool					usrInList = false;
  std::list<Client*>::iterator		it;
  userInfo				*usr = (reinterpret_cast<userInfo*>(data));  

  std::cout << "_handleAuth" << std::endl;
  // Check an existing login in the list
  for (it = _clients.begin(); it != _clients.end(); ++it)
    {
      std::cout << "handleAuth loop looking for user" << std::endl;
      userInfo	* tmp = (*it)->getUserInfo();
      std::string slogin = tmp->login; //used for comparison
      if ((*it) != cl && tmp->token > 0 && slogin == usr->login)
	usrInList = true;
    }
  if (usrInList) {
    std::cout << "handleAuth error" << std::endl;
    cl->pushError(tcp::error::E_AUTH); // the client is in the list, send error.
  }
  else
    {
      std::cout << "Sending token" << std::endl;
      // the client is not in the list o/.
      Token		token;
      token = genToken();
      cl->setUserInfo(*usr);  
      cl->updateIp();
      cl->getUserInfo()->token = token;
      log << "New user " << cl->getUserInfo()->login << std::endl;
      std::cout << "New user " << cl->getUserInfo()->login << std::endl;
      std::cout << "Token " <<  cl->getUserInfo()->token << std::endl;
      std::cout << "Status  " << cl->getUserInfo()->state << std::endl;
      cl->pushAnswer(tcp::R_SETTOKEN, &token, sizeof(token));
      // Send info to all clients.
      for (it = _clients.begin(); it != _clients.end(); ++it)
	{
	  if ((*it) != cl)
	    (*it)->pushAnswer(tcp::R_USERINFO, cl->getUserInfo(), sizeof(userInfo));
	}
    }
  std::cout << "handleAuth finished" << std::endl;
}

/**
 ** _sendUserList (answers to R_GETUSERLIST with R_USERLIST)
 ** Sends the list of all users connected (except the user cl asking for the list)
 **
 **/
void		Server::_sendUserList(Client *cl, void *data)
{
  std::list<Client*>::iterator	it;
  uint32_t	nbusers = _clients.size() - 1;
  int		offset;
  int		size = sizeof(nbusers) + (nbusers * sizeof(userInfo));
  uint8_t	*userlist = new uint8_t[size];


  std::cout << "Sending User List" << std::endl;
  (void)data;
  offset = sizeof(nbusers);
  memcpy(userlist, &nbusers, sizeof(nbusers));
  for (it = _clients.begin(); it != _clients.end(); ++it)
    {
      std::string		login = (*it)->getUserInfo()->login;
      
      if (login != cl->getUserInfo()->login)
	{
	  userInfo	*tmp = (*it)->getUserInfo();

	  std::cout << "[Adding to userlist :]" << std::endl <<
	    "login : " << tmp->login << std::endl << 
	    "ip    : " << tmp->ip  << std::endl;
	  (*it)->updateIp();
	  memcpy(userlist + offset, tmp, sizeof(userInfo));
	  offset += sizeof(userInfo);
	}
    }
  cl->pushAnswer(tcp::R_USERLIST, userlist, size);
  delete [] userlist;
}

/**
 ** _sendUserInfo (answers to R_GETUSERINFO with R_USERINFO)
 ** Sends the userinfo of a specific user
 **
 **/
void		Server::_sendUserInfo(Client *cl, void *data)
{
  userInfo			*ui_to_send;
  Login				login;

  std::cout << "R_GETUSERINFO" << std::endl;
  memcpy(&login, data, sizeof(Login));
  ui_to_send = getClientByLogin(login)->getUserInfo();
  if (ui_to_send)
    {
      cl->pushAnswer(tcp::R_USERINFO, ui_to_send, sizeof(*ui_to_send));
    }
  else
    {
      cl->pushError(tcp::error::E_INVUSR);
    }
}


/**
 ** _chgUserState (callback for R_SETSTATE, doesn't send anything)
 ** modifies the state of a user
 **/
void		Server::_chgUserState(Client *cl, void *data)
{
  uint8_t         st;

  std::cout << "R_SETSTATE" << std::endl;
  memcpy(&st, data, sizeof(st));
  if (st <= 2)
    {
      std::list<Client*>::iterator	it;
      userInfo				usr;

      memcpy(&usr, cl->getUserInfo(), sizeof(usr));
      usr.state = st;
      for (it = _clients.begin(); it != _clients.end(); ++it)
	{
	  if ((*it) != cl)
	    {
	      (*it)->updateIp();
	      (*it)->pushAnswer(tcp::R_USERINFO, &usr, sizeof(usr));
	    }
	}
    }
  else
    cl->pushError(tcp::error::E_INVSTATE);
}

/**
 ** _callEnd (signals the correspondant that the call was closed by the other end)
 **
 **/
void		Server::_callEnd(Client *cl, void *data)
{
  Login		login;
  Client	*correspondant;
  
  std::cout << "CALLEND" << std::endl;
  memcpy(&login, data, sizeof(login));
  correspondant = getClientByLogin(login);
  //  if (!cl->isInCall())
  //return cl->pushError(tcp::error::E_DENIED);
  if (!isInList(_to_be_deleted, cl))
    cl->setCall(0);
  if (correspondant != NULL && !isInList(_to_be_deleted, correspondant))
    {
      correspondant->pushAnswer(tcp::R_CALLENDED, cl->getUserInfo()->login,
				sizeof(Login));
      correspondant->setCall(0);
    }
}

/**
 ** _handleCall ( asks the reciever of the call for an endpoint)
 ** //@TODO reverifier les regles de conversation
 **/
void		Server::_handleCall(Client *cl, void *data)
{
  Bool		connection_refused = 0;
  Port		bad_port = 0;
  Login		login;
  uint8_t	direct_reply[sizeof(Bool) + sizeof(Port)];
  Client	*reciever;

  std::cout << "[handleCall]" << std::endl;
  memcpy(&login, data, sizeof(login));
  reciever = getClientByLogin(login);
  if (reciever == NULL || reciever->getUserInfo()->token <= 0)
    { 
      std::cout << "The reciever doesn't exist or is not authentificated properly"
		<< std::endl;
      return cl->pushError(tcp::error::E_INVUSR);
    }
  if (reciever->isInCall()) //
    {
      std::cout << "The reciever is already in a call" << std::endl;
      memcpy(direct_reply, &connection_refused, sizeof(Bool));
      memcpy(direct_reply + sizeof(Bool), &bad_port, sizeof(Port));
      return cl->pushAnswer(tcp::R_CALLENDPOINT, direct_reply, sizeof(direct_reply));
    }
  std::cout << "The reciever is available for contact : calling" << std::endl;
  _conversations.push_back(conversation(cl, reciever, WAITING_FOR_ENDPOINT));
  cl->setCall(1);
  reciever->setCall(1);
  reciever->pushAnswer(tcp::R_RECVCALL, cl->getUserInfo(), sizeof(userInfo));
}

/**
 ** _handleDisconnect
 ** Closes a client connection and removes it from the list
 ** @TODO //signaler aux autres que le cl s'est deconnecter (ne pas le signaler a lui meme)
 **/
void		Server::_handleDisconnect(Client *cl, void *data)
{
  std::cout << "R_DISCONNECT from : " << cl->getUserInfo()->login << std::endl;
  (void)data;
  addToDeleteList(cl);
}

/**
 ** _forwardCallEndpoint (forwards the R_CALLENDPOINT to the caller)
 **
 **/
void		Server::_forwardCallEndpoint(Client *cl, void *data)
{
  conversation	*cv;

  std::cout << "CALLENDPOINT (forwardCallEndpoint)" << std::endl;
  if (!cl->isInCall() ||
      ((cv = getConversationByReciever(cl)) == NULL) ||
      cv->state == IN_CALL)
    return cl->pushError(tcp::error::E_DENIED); 
  cv->caller->pushAnswer(tcp::R_CALLENDPOINT, data, sizeof(Bool) +  sizeof(Port));
}

/**
 ** _handleDisconnect (answers to R_PING with R_PONG)
 ** ping/pong
 */
void		Server::_handlePing(Client *cl, void *data)
{
  (void)cl;
  (void)data;
  cl->pushAnswer(tcp::R_PONG, NULL, 0);
}

/**
 ** _handlePong
 ** updates timestamp
 */
void		Server::_handlePong(Client *cl, void *data)
{
  (void)cl;
  (void)data;
  std::cout << "Recieved Pong" << std::endl;
  cl->setTimestamp(0);
}

/**
 ** Generates a new token
 **/
Token		Server::genToken()
{
  Token			_token;

  _token = (rand() % 1000) + 1;
  std::list<Client*>::iterator	it;
  for (it = _clients.begin(); it != _clients.end(); ++it)
    {
      if ((*it)->getUserInfo()->token == _token)
	return genToken();
    }
  return (_token);
}
