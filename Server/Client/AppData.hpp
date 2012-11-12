#pragma once

#include <iostream>
#include <stdint.h>

//TODO: ad:
//			- DB Class
//			- Image Class

class AppData {
public:
	public:
		std::string _appDirectory;
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

