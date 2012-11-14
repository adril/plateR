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

//TODO: ad:
//			- DB Class
//			- Image Class

class AppData {
public:
	public:
		std::string		_appDirectory;
		sql::Driver		*_SQLdriver;
		sql::Connection *_dataBaseConnector;
	  public:
        static AppData& getInstance() {
            static AppData    instance;
            return instance;
        }
    private:
        AppData();
        AppData(AppData const&);
        void operator=(AppData const&);
};

#endif