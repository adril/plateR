#pragma once

#include <iostream>
#include <stdint.h>

//TODO: ad:
//			-Misc tools

class AppData {
public:
	public:
		std::string _port;
		std::string _host;
		std::string	_appDirectoryPath;
		std::string	_fileDirectory;
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

