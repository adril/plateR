#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <stdint.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

#define DISCONNECTION_DELAY_S 120
#define INACTIVITY_DELAY_S DISCONNECTION_DELAY_S/4
#define DEFAULT_PORT 25493
#define	BUF_SIZE 4096

typedef char Login [25];
typedef char Password [25];
typedef uint32_t Token;
typedef char IP [17];
typedef uint16_t Port;

namespace tcp
{
	typedef boost::system::error_code error_code;
	typedef boost::asio::ip::tcp::socket socket;
	typedef boost::asio::ip::tcp::acceptor acceptor;
	typedef boost::asio::io_service io_service;
	typedef boost::asio::ip::tcp::endpoint endpoint;
	typedef boost::asio::deadline_timer deadline_timer;
};

struct buffer_t
{
    uint8_t buffer[BUF_SIZE];
    unsigned int offset;
    buffer_t() {offset = 0;memset(buffer, 0, BUF_SIZE);}
};

#pragma pack(push)
#pragma pack(1)

struct userInfo {
  Login		 login;
  Password   pass;
  Token		 token;
  IP		 ip;
};

#pragma pack(pop)

#endif