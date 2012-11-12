//
// async_tcp_client.cpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <iostream>
#include <fstream>
#include <queue>
#include "LProtocol.hh"
#include "VSP.h"

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;


class client : public IProtocolDelegate
{
public:
	Message *_inputMessage;
	LProtocol	*_protocol;

  client(boost::asio::io_service& io_service)
    : stopped_(false),
      socket_(io_service),
      deadline_(io_service),
      heartbeat_timer_(io_service)
  {
	_protocol = new LProtocol;
	_protocol->_delegate = this;
	_inputMessage = new Message;
  }


  void start(tcp::resolver::iterator endpoint_iter)
  {

    start_connect(endpoint_iter);
    deadline_.async_wait(boost::bind(&client::check_deadline, this));
  }

  void stop()
  {
    stopped_ = true;
    boost::system::error_code ignored_ec;
    socket_.close(ignored_ec);
    deadline_.cancel();
    heartbeat_timer_.cancel();
  }

private:
  void start_connect(tcp::resolver::iterator endpoint_iter)
  {
    if (endpoint_iter != tcp::resolver::iterator())
    {
      std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";
      deadline_.expires_from_now(boost::posix_time::seconds(60));
      socket_.async_connect(endpoint_iter->endpoint(),
          boost::bind(&client::handle_connect,
            this, _1, endpoint_iter));
    }
    else
      stop();
  }

  void handle_connect(const boost::system::error_code& ec, tcp::resolver::iterator endpoint_iter)
  {
    if (stopped_)
      return;

    if (!socket_.is_open())
    {
      std::cout << "Connect timed out\n";
      start_connect(++endpoint_iter);
    }
    else if (ec)
    {
      std::cout << "Connect error: " << ec.message() << "\n";
      socket_.close();
      start_connect(++endpoint_iter);
    }
    else
    {
      std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";
	  //sendLogin("jullien", "1234567");
	  //sendLogout();
	  sendFile("Smile-3.jpg");
      start_read();
    }
  }

  void start_read()
  {
	const boost::system::error_code error;
	read(error);
  }

void read(const boost::system::error_code& error) {
	if (error)
	{
		std::cout << "Client::handle_input. ERROR !" << std::endl;
		socket_.close();
	}
	else
		readHeader(error);
}
void readHeader(const boost::system::error_code& error) {
	this->_inputMessage = new Message();
	socket_.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getHeader(), this->_inputMessage->getHeaderLength()),
		boost::bind(&client::readHeaderHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void readBody(const boost::system::error_code& error) {
	socket_.async_read_some(boost::asio::buffer((unsigned char *)this->_inputMessage->getBody(), this->_inputMessage->getBodyLength()),
		boost::bind(&client::readBodyHandler, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void readHeaderHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	//TODO: do job on the message

	std::cout << "{HEADER} bytes_transferred : " << bytes_transferred << std::endl;
	this->_inputMessage->decodeHeader();
	//TODO: if bad header -> 	readheader(error); and clean the Message

	readBody(error);
}

void readBodyHandler(const boost::system::error_code& error, size_t bytes_transferred) {
	std::cout << "{BODY} bytes_transferred : " << bytes_transferred << std::endl;
	this->_protocol->receiveMessage(*this->_inputMessage);
	//delete _inputMessage;
	read(error);
}
  void handle_read(const boost::system::error_code& ec)
  {
    if (stopped_)
      return;

    if (!ec)
    {
      // Extract the newline-delimited message from the buffer.
      std::string line;
      std::istream is(&input_buffer_);
      std::getline(is, line);

      // Empty messages are heartbeats and so ignored.
      if (!line.empty())
      {
        std::cout << "Received: " << line << "\n";
      }

      start_read();
    }
    else
    {
      std::cout << "Error on receive: " << ec.message() << "\n";

      stop();
    }
  }

  std::string readFile(std::string filename)
  {
	unsigned int size = 0;
	char *memblock;

	std::ifstream file (filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
	if (file.is_open())
	{
		size = (unsigned int)file.tellg();
		memblock = new char[size];
		file.seekg (0, std::ios::beg);
		file.read (memblock, size);
		file.close();
		std::string buffer(memblock, size);
		return buffer;
	}
	return NULL;
  }
  void sendMessage(Message &msg)
  {
	  	boost::asio::async_write(socket_, 
		boost::asio::buffer((unsigned char *)msg.getData(), msg.getDataLength()),
		boost::bind(&client::handle_write, this, _1));
  }
  void sendMessageQueue(std::queue<Message*> msgQueue)
  {
	  while (!msgQueue.empty())
	  {
		  sendMessage(*msgQueue.front());
		  msgQueue.pop();
	  }
  }
  void sendLogin(std::string username, std::string password)
  {
	Message *message = new Message;
	message->encodeHeader(VSP::LOGIN);
	message->decodeHeader();

	MessageLogin *customMessage = new MessageLogin(*message,  (char*)username.c_str(), (char*)password.c_str());
	customMessage->encodeBody();
	customMessage->encodeData();
	sendMessage(*customMessage);
  }
  void sendLogout()
  {
	Message *message = new Message;
	message->encodeHeader(VSP::LOGOUT);
	message->decodeHeader();

	MessageLogOut *customMessage = new MessageLogOut(*message);
	customMessage->encodeBody();
	customMessage->encodeData();
	sendMessage(*customMessage);
  }
  void sendFile(std::string filename)
  {
	  std::string buffer = readFile(filename.c_str());
	  std::queue<Message*> messageQueu;
	  unsigned int countMsg = 0, fileSize = buffer.length();
	  size_t	idx = 0, maxIdx = buffer.length(), to_read = 0, to_write = 0;
	  countMsg = buffer.length() > VSP::FILE_SIZE ? buffer.length() / VSP::FILE_SIZE : 1;


	 for (unsigned int i = 0; i <= countMsg; i++)
	  {
		  to_read = maxIdx - idx;
		  to_write = to_read > VSP::FILE_SIZE ? VSP::FILE_SIZE : to_read;
		  Message *message = new Message;
		  MessageFile *customMessage = new MessageFile(*message, 'c', idx, maxIdx, (char*)filename.c_str(), to_read, (char*)buffer.c_str() + idx);

		  message->encodeHeader(VSP::FILE);
		  message->decodeHeader();
		  customMessage->encodeBody();
		  customMessage->encodeData();
		  messageQueu.push(customMessage);

		  idx += to_write;
		  to_read = maxIdx - idx;
	  }
	sendMessageQueue(messageQueu);
  }


  void start_write()
  {
	/*boost::asio::async_write(socket_, 
		boost::asio::buffer((unsigned char *)customMessage->getData(), customMessage->getDataLength()),
		boost::bind(&client::handle_write, this, _1));*/
	 //socket_.close();

  }

  void handle_write(const boost::system::error_code& ec)
  {
    if (stopped_)
      return;

    if (!ec)
    {
      // Wait 10 seconds before sending the next heartbeat.
      heartbeat_timer_.expires_from_now(boost::posix_time::seconds(10));
      heartbeat_timer_.async_wait(boost::bind(&client::start_write, this));
    }
    else
    {
      std::cout << "Error on heartbeat: " << ec.message() << "\n";

      stop();
    }
  }

  void check_deadline()
  {
    if (stopped_)
      return;

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (deadline_.expires_at() <= deadline_timer::traits_type::now())
    {
      // The deadline has passed. The socket is closed so that any outstanding
      // asynchronous operations are cancelled.
      socket_.close();

      // There is no longer an active deadline. The expiry is set to positive
      // infinity so that the actor takes no action until a new deadline is set.
      deadline_.expires_at(boost::posix_time::pos_infin);
    }

    // Put the actor back to sleep.
    deadline_.async_wait(boost::bind(&client::check_deadline, this));
  }

  void loginHandler(Message &message) 
{

}

void loginResultHandler(Message &message) 
{
	std::cout << "OK" << std::endl;// none
}

void logOutHandler(Message &message) 
{
	// kill client
}

void plateHandler(Message &message) 
{
	//none
}

void fileHandler(Message &message) 
{
	std::cout << "FILE" << std::endl;
}

void unknowMessageHandler(Message &message) {

}

/* Message */

Message *headerMessage(char bodyType) {
	Message *message = new Message();
	message->encodeHeader(bodyType);
	message->decodeHeader();
	return message;
}
private:
  bool stopped_;
  tcp::socket socket_;
  boost::asio::streambuf input_buffer_;
  deadline_timer deadline_;
  deadline_timer heartbeat_timer_;
};


int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;
    tcp::resolver r(io_service);
    client c(io_service);

    c.start(r.resolve(tcp::resolver::query("127.0.0.1", "25493")));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}