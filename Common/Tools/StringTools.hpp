#ifndef _STRINGTOOLS_HPP_
#define _STRINGTOOLS_HPP_

#include <iostream>
#include <string>
#include <sstream>

class StringTools {
public :
	static void copyString(char *src, char *dest, size_t size);
	static std::string StringTools::UrlEncode(const std::string& szToEncode);
	static std::string StringTools::UrlDecode(const std::string& szToDecode);
	static std::string integerToString(int value);
	static std::string stringToMd5(std::string value);
};

#endif /* _FILETOOLS_HPP_ */