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
//#define DEFAULT_PORT 4242//25493
#define	BUF_SIZE 4096

namespace tcp
{
	typedef boost::system::error_code error_code;
	typedef boost::asio::ip::tcp::socket socket;
	typedef boost::asio::ip::tcp::acceptor acceptor;
	typedef boost::asio::io_service io_service;
	typedef boost::asio::ip::tcp::endpoint endpoint;
	typedef boost::asio::deadline_timer deadline_timer;
};

#endif