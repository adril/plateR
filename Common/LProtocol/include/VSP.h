#ifndef __VSP_H__
#define __VSP_H__

#define IS_IN_RANGE(X, Y, Z) (X >= Y ? (X <= Z ? true : false) : false)

namespace VSP {
  const size_t PORT = 4242;
  const size_t LOGIN_SIZE = 25;
  const size_t PASSWORD_SIZE = 25;
  const size_t NAME_SIZE = 25;
  const size_t CODE_PLATE_SIZE = 25;
  const size_t CODE_FILE_SIZE = 25;
  const size_t FILE_SIZE = 4096;

  enum HEADER {
    LOGIN,
    LOGIN_RESULT,
    LOGOUT,
    PLATE,
    FILE
  };

  enum PLATE_STATE {
    RECORDED,
    LIVE
  };

  enum STATE {
    KO,
    OK
  };

  enum PERROR {
    ERROR_NOTHING,
    ERROR_FILE
  };

# pragma pack(push, 1) /* Byte align is strict from 1 byte */

  typedef struct  Header {
    char type;
    size_t body_size;//size_struct;
    char error;
  } Header;

  typedef struct  Login {
    char type;
    char login[LOGIN_SIZE];
    char password[PASSWORD_SIZE];
  } Login;

  typedef struct LoginResult {
    char type;
    char state;//INFO: STATE
  } LoginResult;

  typedef struct LogOut {
    char type;
  } LogOut;

  typedef struct Plate {
    char type;
    char code_plate[CODE_PLATE_SIZE];
    char name[NAME_SIZE];
	char code_file[CODE_FILE_SIZE];
    char state;//INFO: PLATE_STATE
  } Plate;

  /* Data transfert */

  typedef struct File {
    char type;
    size_t indx;
    size_t max_indx;
	char code_file[CODE_FILE_SIZE];
    size_t to_read;
    char file[FILE_SIZE];
  } File;

# pragma pack(pop)

}

#endif /* !__VSP_H__ */
