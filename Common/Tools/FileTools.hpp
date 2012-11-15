#ifndef _FILETOOLS_HPP_
#define _FILETOOLS_HPP_

#include <iostream>


class FileTools {
public :
	static bool writeStringToFile(std::string fileContent, std::string filePath);
	static std::string readStringFromFile(std::string filePath);
	static bool fileExistAtPath(std::string path);
	static bool createDirectoryAtPath(std::string path);
	static std::string epurPath(std::string path);
};

#endif /* _FILETOOLS_HPP_ */