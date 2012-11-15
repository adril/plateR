#ifndef _APP_DATA_HPP_
#define _APP_DATA_HPP_

#pragma once
#pragma comment(lib, "mysqlcppconn.lib")

#include <iostream>
#include <stdint.h>

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn\exception.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\sqlstring.h>
#include <cppconn\prepared_statement.h>

#define HOST "tcp://127.0.0.1:3306"
#define USER "root"
#define PASS ""
#define DBNAME "softwareproj"

#include <iostream>
#include <stdint.h>

class AppData {
public:
	public:
		std::string		_appDirectoryPath;
		std::string		_fileDirectory;
		sql::Driver		*_SQLdriver;
		sql::Connection *_dataBaseConnector;
	  public:
        static AppData& getInstance() {
            static AppData    instance;
            return instance;
        }
		std::string getFileDirectoryPath();
    private:
        AppData();
        AppData(AppData const&);
        void operator=(AppData const&);
};

#endif