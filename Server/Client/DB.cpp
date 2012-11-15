#include "DB.hpp"
#include "FileTools.hpp"
#include <string>


bool	DB::add_user(DBDATA::User user)
{
	std::string	query = 
	"INSERT if not exists INTO `user` (`id` ,`name` ,`password`) VALUES (NULL , '" + user.username + "', '" + user.password + "');"; 
	return DB::execute(query);
}
bool	DB::add_file(DBDATA::File file)
{
	std::string	query = 
		"INSERT INTO `file` (`id` ,`name` ,`path`, `codeFile`, `user`, `state`) VALUES (NULL , '" 
		+ file.name + "', '"
		+ file.path + "', '"
		+ file.codeFile + "', '"
		+ file.user + "', '0');";
	FileTools::writeStringToFile(query, "query");
	return DB::execute(query);
}
bool	DB::add_plate(DBDATA::Plate plate)
{
	std::string	query = 
		"INSERT INTO `plate` (`id` ,`name` ,`codePlate`, `codeFile`, `path`, `user`, `state`) VALUES (NULL , '" 
		+ plate.name + "', '"
		+ plate.codePlate + "', '"
		+ plate.codeFile + "', '"
		+ plate.path + "', '"
		+ plate.user + "', '0');"; 
	return DB::execute(query);
}


bool DB::update_file(DBDATA::File file)
{
	std::string	query = 
		"UPDATE `file` SET `name` = '" 
		+ file.name + "', `path` = '" 
		+ file.path + "', `user` = '" 
		+ file.codeFile + "', `codeFile` = '" 
		+ file.user +"', `state` = '"
		+ std::to_string(file.state) + "' WHERE `name` = '"
		+ file.name + "';";

	return DB::execute(query);
}
bool update_plate(DBDATA::Plate)
{
	return true;
}

std::list<DBDATA::File> DB::get_file_to_treat(std::string username)
{
	//name 	path 	user 	state
	std::list<DBDATA::File> fileList;
	std::string		query = "SELECT name, path, codeFile, user, state FROM file WHERE user='";
	query += username + "' AND state = 0;";
	try
	{
		sql::ResultSet *res = DB::executeQuery(query);
		while (res->next())
		{
			fileList.push_back(DBDATA::File(res->getString("name"), res->getString("path"), res->getString("codeFile"), res->getString("user"), res->getInt("state")));
		}
		return fileList;
	}
	catch (sql::SQLException *e)
	{
		return fileList;

	}
}

DBDATA::User* DB::get_user_by_login(std::string username)
{
	std::string		query = "SELECT name, password FROM user WHERE name='";
	query += username + "';";

	try
	{
		sql::ResultSet *res = DB::executeQuery(query);
		if (res->next())
			return new DBDATA::User(res->getString("name"), res->getString("password"));
		else
			return NULL;
	}
	catch (sql::SQLException *e)
	{
		return NULL;

	}
}
std::list<DBDATA::Plate> DB::get_all_recorded_plate(std::string username)
{
	std::list<DBDATA::Plate> fileList;	
	std::string		query = "SELECT * FROM plate WHERE user='";
	query += username + "' AND state = 0;";

	try
	{
		sql::ResultSet *res = DB::executeQuery(query);
		while (res->next())
		{
			fileList.push_back(DBDATA::Plate(res->getString("codePlate"), res->getString("name"), res->getString("codeFile"), res->getString("path"), res->getString("user"), res->getInt("state")));
		}
		return fileList;
	}
	catch (sql::SQLException *e)
	{
		return fileList;

	}
}

/* INTERNAL FUNCTION */

sql::ResultSet* DB::executeQuery(std::string query)
{
	try
	{
		sql::Statement *stmt = AppData::getInstance()._dataBaseConnector->createStatement();
		return (stmt->executeQuery(query));
	}
	catch(sql::SQLException *e)
	{
		std::cout << e->what() << std::endl;
		return NULL;
	}	
}
bool DB::execute(std::string query)
{
	try
	{
		sql::Statement *stmt = AppData::getInstance()._dataBaseConnector->createStatement();
		return (stmt->execute(query));
	}
	catch(sql::SQLException *e)
	{
		std::cout << e->what() << std::endl;
		return NULL;
	}	
}