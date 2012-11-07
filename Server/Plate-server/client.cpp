#include "client.hpp"

////////////////////////
///// CONSTRUCTION /////
////////////////////////

Client::Client(tcp::io_service& io_service, Server *s)
  : _socket(io_service), _server(s)
{
  // Size association
  memset(_requestDataSize, 0, sizeof(_requestDataSize));
  _requestDataSize[tcp::R_AUTH] = sizeof(userInfo);
  _requestDataSize[tcp::R_GETUSERINFO] = sizeof(Login);
  _requestDataSize[tcp::R_SETSTATE] = sizeof(uint8_t);
  _requestDataSize[tcp::R_CALLUSER] = sizeof(Login);
  _requestDataSize[tcp::R_CALLENDED] = sizeof(Login);
  _requestDataSize[tcp::R_CALLENDPOINT] = sizeof(Bool) + sizeof(Port);
  // Init
  _inCall = false;
  _timestamp = 0.0;
  _req_size_left = 0;
  _req_size = 0;
  _userInfo.state = Online;
  memset(_userInfo.location, 0, sizeof(Location));
  memset(_userInfo.login, 0, sizeof(Login));
  memcpy(_userInfo.login, "nameless", sizeof("nameless"));
  _userInfo.token = -1;
}

Client::~Client()
{
  boost::system::error_code err;

    _socket.cancel(err);
    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
    _socket.close(err);
}

//first read
void	Client::start()
{
  updateIp();
  _socket.async_read_some(boost::asio::buffer(_input_buffer.buffer, BUF_SIZE),
			  boost::bind(&Client::handle_input, this,
				      boost::asio::placeholders::error,
				      boost::asio::placeholders::bytes_transferred));
}

void	Client::updateIp()
{
  memset(_userInfo.ip, 0, 17);
  memcpy(_userInfo.ip,
	 _socket.remote_endpoint().address().to_string().c_str(),
	 _socket.remote_endpoint().address().to_string().size());
  std::cout << "Updating ip : " << _userInfo.ip << std::endl;
}

///////////////
///// I/O /////
///////////////

void Client::handle_input(const boost::system::error_code& error,
			  size_t bytes_transferred)
{
  int		to_read;
  uint8_t	req_as_uint8;

  std::cout << "\n==================NEW READ==================" << std::endl;
  std::cout << std::endl << "Client::handle_input : read "
	    << bytes_transferred << " bytes" << std::endl;
  /**
   ** Reads through the data recieved, and retrieves each request with it's data
   ** If the data is incomplete, the request buffer will wait the next async read
   ** for completion
   **/
  _input_buffer.offset = 0;
  while (_input_buffer.offset != bytes_transferred)
    {
      if (_req_size_left == 0)
	{	 
	  // The last request has been dealt with
	  // Starting the next one
	  memcpy(&req_as_uint8, _input_buffer.buffer + _input_buffer.offset,
		 sizeof(req_as_uint8));
	  _currentRequest = static_cast<tcp::Request>(req_as_uint8);
	  if (_currentRequest < 0 || _currentRequest > tcp::R_PONG)
	    {
	      _server->log << "[ERROR UNKNOWN REQUEST] : " << (int)req_as_uint8 << " " << _currentRequest
			<< " from " << _userInfo.login << std::endl;
	      std::cout << "ERROR : unknown request : " << _currentRequest
			<< " from " << _userInfo.login << std::endl;
		  _server->addToDeleteList(this);
	      return;
	    }
	  _input_buffer.offset += sizeof(req_as_uint8);
	  _request_buffer.offset = 0;
	  _req_size = _requestDataSize[_currentRequest];
	  _req_size_left = _req_size;
	  std::cout << "New Request : " << _currentRequest << " with "
		    << _req_size << " bytes of data" << std::endl;
	  if (_req_size_left == 0) // No data, pushing
	    {	     
	      std::cout << "No data needed for this request, calling _handleRequest" << std::endl;
	      _server->handleRequest(this, _currentRequest, NULL);
	    }
	}
      else
	{
	  // The current request data is incomplete
	  if (_req_size_left <= (bytes_transferred - _input_buffer.offset))
	    {
	      // The data is present in the buffer
	      to_read = _req_size_left;
	    }
	  else
	    {
	      // The data is only partially present in the buffer, reading everything left
	      to_read = (bytes_transferred - _input_buffer.offset);
	    }
	  memcpy(_request_buffer.buffer + _request_buffer.offset,
		 _input_buffer.buffer + _input_buffer.offset,
		 to_read);
	  _req_size_left -= to_read;
	  _input_buffer.offset += to_read;
	  _request_buffer.offset += to_read;
	  if (_req_size_left == 0)
	    {
	      std::cout  << "All the data for request " << _currentRequest <<
		" has been read, calling _handleRequest" << std::endl;
	      // Data has been completed, pushing the request
	      uint8_t *data = new uint8_t[_req_size];
	      memcpy(data, _request_buffer.buffer, _req_size);
	      _server->handleRequest(this, _currentRequest, data);
	      delete [] data;
	    }
	}
      std::cout << "[bytes_analyzed/bytes_recieved] " << _input_buffer.offset 
		<< "/" << bytes_transferred << std::endl;	    
    }
  std::cout << "============================================" << std::endl;
  read(error);
}


  void Client::read(const boost::system::error_code& error)
  {
    if (!error)
      {
	_socket.async_read_some(boost::asio::buffer(_input_buffer.buffer, BUF_SIZE),
				boost::bind(&Client::handle_input, this,
					    boost::asio::placeholders::error,
					    boost::asio::placeholders::bytes_transferred));
      }
    else
      {
	_server->addToDeleteList(this);
      }
  }

  void Client::handle_output(uint8_t *data, int len)
  { 
    std::cout << "Sending " << len << " bytes." << std::endl;
    boost::asio::async_write(this->_socket,
			     boost::asio::buffer(data, len),
			     boost::bind(&Client::writeFinish, this,
					 boost::asio::placeholders::error, data));
  }


  void Client::writeFinish(const boost::system::error_code &error, uint8_t *data)
  {
    if (data != NULL)
      delete[] data;
    if (!error)
      std::cout << "Client::handle_output. SUCESS !" << std::endl;
    else
      {
	std::cout << "Client::handle_output. ERROR !" << std::endl;
	_server->addToDeleteList(this);
      }
    //@TODO kill client on error ??
  }

  void Client::pushAnswer(tcp::Request req, void *data, size_t datalen)
  {
    int			len = sizeof(uint8_t) + ((data != NULL) ? datalen : 0);
    uint8_t		*paquet = new uint8_t[len + 1];
    uint8_t		req_as_uint8 = req;
  
	memset(paquet, 0, len);
    memcpy(paquet, &req_as_uint8, sizeof(req_as_uint8));
    if (data != NULL && datalen != 0)
      memcpy(paquet + sizeof(req_as_uint8), data, datalen);
    this->handle_output(paquet, len);
  }

  void Client::pushError(tcp::error::ServerErrorCode err)
  {
    uint8_t		req = tcp::R_ERROR;
    uint8_t		err_as_uint8 = err;
    int			len = sizeof(req) + sizeof(err_as_uint8);
    uint8_t		*paquet = new uint8_t[len];

    memcpy(paquet, &req, sizeof(req));
    memcpy(paquet + sizeof(req), &err_as_uint8, sizeof(err_as_uint8));
    this->handle_output(paquet, len);
  }

  ///////////////////
  ///// GETTER /////
  ///////////////////

  tcp::socket&	Client::getSocket()
  {
    return _socket;
  }

  std::string	Client::getIpAsString() const
  {
    return _socket.remote_endpoint().address().to_string();
  }

  double		Client::getTimestamp() const
  {
    return _timestamp;
  }

  void		Client::setTimestamp(double t)
  {
    _timestamp = t;
  }

  bool		Client::isInCall()
  {
    return _inCall;
  }

  // userInfo

  void		Client::setUserInfo(userInfo &u)
  {
    memcpy(&_userInfo, &u, sizeof(_userInfo));
  }

  userInfo	*Client::getUserInfo()
  {
    return (&this->_userInfo);
  }

  void		Client::setCall(bool c)
  {
    _inCall = c;
  }
