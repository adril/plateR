#include "AppData.hpp"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include "FileTools.hpp"

AppData::AppData() {
	std::cout << "AppData::AppData(void)" << std::endl;

	TCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path))) {
		this->_appDirectoryPath = FileTools::epurPath(path);
	}
	else {
		this->_appDirectoryPath = "";
	}
	this->_fileDirectory = "/to_analyze/";

}

std::string AppData::getFileDirectoryPath() {
	std::string result = this->_appDirectoryPath + this->_fileDirectory;
	return result;
}


