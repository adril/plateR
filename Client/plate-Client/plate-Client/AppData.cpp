#include "AppData.hpp"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include "FileTools.hpp"

AppData::AppData() {
	std::cout << "AppData::AppData(void)" << std::endl;

	this->_host = "127.0.0.1";
	this->_port = "4243";

	TCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path))) {
		this->_appDirectoryPath = FileTools::epurPath(path);
	}
	else {
		this->_appDirectoryPath = "";
	}
	this->_fileDirectory = "/analyzed/";
}

std::string AppData::getFileDirectoryPath() {
	std::string result = this->_appDirectoryPath;
	result += this->_fileDirectory;
	return result;
}


