/**
 ** Babel Server
 ** CLIENT.HPP
 **
 **/

#ifndef CLIENT_BABEL_H
#define CLIENT_BABEL_H

#include "common.hpp"
#include "server.hpp"

#define	BUF_SIZE 4096

class Server;
class Client
{
public :

  struct buffer_t
  {
    uint8_t buffer[BUF_SIZE];
    unsigned int offset;
    buffer_t() {offset = 0;memset(buffer, 0, BUF_SIZE);}
  };
    

  Client(tcp::io_service& io_service, Server *s);
  ~Client();

  // Getters
  tcp::socket&	getSocket();
  std::string	getIpAsString() const;
  userInfo	*getUserInfo();
  double	getTimestamp() const;
  void		pushAnswer(tcp::Request, void*, size_t);
  void		pushError(tcp::error::ServerErrorCode);
  bool		isInCall();
  // Setters
  void		setUserInfo(userInfo &);
  void		setTimestamp(double t);
  void		setCall(bool);

  void start();
  void updateIp();

private :
  // Default contructor cannot be used
  Client();

  // Internal IO functions
  void read(const boost::system::error_code& error);
  void writeFinish(const boost::system::error_code &error, uint8_t *data);
  
  void handle_output(uint8_t*, int);
  void handle_input(const boost::system::error_code& error, size_t bytes_transferred);

private :

  // Protocol:
  typedef unsigned int	requestDataSize[REQUESTCOUNT];

  requestDataSize		_requestDataSize;

  // Network:
  tcp::socket			_socket;
  Server		*	_server;

  // I/O :
  buffer_t			_input_buffer; // Main buffer
  buffer_t			_output_buffer; // Output buffer
  buffer_t			_request_buffer; // Second buffer used for storing
  tcp::Request			_currentRequest; // Current Request stored
  size_t			_req_size; // Size of the data associated with the request
  size_t			_req_size_left; // Size of the missing data

  // Client data
  userInfo			_userInfo;
  double			_timestamp;
  bool				_inCall;
};

#endif
