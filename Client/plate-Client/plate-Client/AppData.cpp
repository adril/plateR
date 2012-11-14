#include "AppData.hpp"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

AppData::AppData() {
	std::cout << "AppData::AppData(void)" << std::endl;
	//TODO: get AppDirectory [KO]
	TCHAR szPath[MAX_PATH];
	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath ) ) ) {
		this->_appDirectory = szPath;//"hello world";
	}
	this->_host = "127.0.0.1";
	this->_port = "4242";
}


