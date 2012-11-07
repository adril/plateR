#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <stdint.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

typedef char Login [25]; // 25 caractères max ?
typedef char Location [25];
typedef uint32_t Token; // > 0, (-1 en cas d’erreur)
typedef char IP [17];
typedef uint16_t Port;
typedef uint32_t NbUsers;
typedef uint8_t Bool;

enum State {
  Online,
  Away,
  Busy
};

#pragma pack(push)
#pragma pack(1)

struct userInfo {
  Login		 login;
  Token		 token;
  IP		 ip;
  uint8_t        state;
  Location	location; // Du genre “ClientKikouBabel”
};
/*
struct          userInfo
{
  Login         login;
  Location      location;
  uint8_t	state;
  Token         token;
  IP            ip;
  };*/

#pragma pack(pop)

namespace tcp
{
  enum Request
    {
      R_AUTH,
      R_GETUSERLIST,
      R_GETUSERINFO,
      R_SETSTATE,
      R_CALLUSER,
      R_DISCONNECT,
      R_DISCONNECTED,
      R_CALLENDED,
      R_SETTOKEN,
      R_TIMEOUT,
      R_USERLIST,
      R_USERINFO,
      R_RECVCALL,
      R_CALLENDPOINT,
      R_ERROR,
      R_PING,
      R_PONG,
      R_MSG,
      R_FILE,
      R_SHARE,
      R_SETLOCATION,
      R_PUB
    };

  typedef boost::system::error_code error_code;
  typedef boost::asio::ip::tcp::socket socket;
  typedef boost::asio::ip::tcp::acceptor acceptor;
  typedef boost::asio::io_service io_service;
  typedef boost::asio::ip::tcp::endpoint endpoint;

  namespace error
  {
    enum ServerErrorCode {
      E_AUTH = 1 << 4,// R_AUTH a echoue
      E_AUTH_DENIED = E_AUTH | 1,// N’est pas autorise a se R_AUTH
      E_AUTH_ALRDLOG = E_AUTH | 2,// A deja un token attribue
      E_AUTH_WRGLOG = E_AUTH | 3,// Login invalide
      E_AUTH_UNVLOG = E_AUTH | 4,// Login déjà pris
      E_INVSTATE = 2 << 4,// Status invalide
      E_INVUSR = 3 << 4,// Le login specifie est invalide (non connecte,...)
      E_INVCMD = 4 << 4,// Commande invalide
      E_INVARG = 5 << 4,// Argument invalide (erreur generique)
      E_DENIED = 6 << 4,// N’est pas autorise a executer la commande (erreur generique)
      E_ERROR = 7 << 4 // Erreur generique
    };
  }
}

#define REQUESTCOUNT tcp::R_PONG + 1
#define DEFAULT_PORT 25493

#endif
