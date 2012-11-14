#include "AppData.hpp"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

AppData::AppData() {
	std::cout << "AppData::AppData(void)" << std::endl;
	//TODO: get AppDirectory [OK]
	TCHAR szPath[MAX_PATH];
	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_MYDOCUMENTS, NULL, 0, szPath ) ) ) {
		this->_appDirectory = szPath; //"hello world";

	}

}


