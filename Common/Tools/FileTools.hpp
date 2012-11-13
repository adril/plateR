#ifndef _FILETOOLS_HPP_
#define _FILETOOLS_HPP_

#include <iostream>


class FileTools {
public :
	static bool writeStringToFile(std::string fileContent, std::string filePath);
	static std::string readStringFromFile(std::string filePath);
};

#endif /* _FILETOOLS_HPP_ */