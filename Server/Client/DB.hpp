#ifndef _DB_HPP_
#define _DB_HPP_

#include "AppData.hpp"

namespace DBDATA {
	struct User {
		std::string username;
		std::string password;
		User(std::string user, std::string pass) : username(user), password(pass){}
		bool authUser(std::string pass) { return password == pass; };
	};
	struct Plate {
		std::string codePlate;//INFO: plate value
		std::string name;
		std::string codeFile;//INFO: foreign key between a plate and a file
		std::string path;
		std::string user;
		int state;
		Plate(std::string cdplate, std::string nam, std::string cdfile, std::string p, std::string u, int s = 0)
			: codePlate(cdplate), name(nam), codeFile(cdfile), path(p), user(u), state(s) {}
	};
	struct File {
		std::string name;
		std::string path;
		std::string user;
		int state;
		File(std::string n, std::string p, std::string u, int s = 0)
			: name(n), path(p), user(u), state(s) {}
	};
};

class DB
{
public :
	static bool add_user(DBDATA::User);
	static bool add_file(DBDATA::File);
	static bool add_plate(DBDATA::Plate);

	static bool update_file(DBDATA::File);
	static bool update_plate(DBDATA::Plate);
	//static bool add_plate(DBDATA::Plate);

	static DBDATA::User* get_user_by_login(std::string);
	static std::list<DBDATA::File> get_file_to_treat(std::string);
	static std::list<DBDATA::Plate> get_all_recorded_plate(std::string);
	/*static DBDATA::Plate get_plate_by_name(std::string);
	static DBDATA::Plate get_plate_by_codePlate(std::string);
	static DBDATA::Plate get_plate_by_codeFile(std::string);
	*/


	//static bool add_plate(DBDATA::Plate);
	//static bool	get_all_user();
	//static bool	get_user_by_name(std::string name);
	//static bool	add_plate(std::string number, std::string picture_path);
	//static bool	get_plate_by_id(std::string id);
	//static bool	get_plate_by_number(std::string number);
	//static bool	get_plate_by_picture_path(std::string picture_path);
	//static bool	get_all_plate();
	//static bool	get_user_by_id(std::string id);
private :
	static sql::ResultSet* executeQuery(std::string);
	static bool execute(std::string);
};

#endif
