#ifndef _STRINGTOOLS_HPP_
#define _STRINGTOOLS_HPP_
#include <string>

class StringTools {
public :
	static void copyString(char *src, char *dest, size_t size);
	static std::string StringTools::UrlEncode(const std::string& szToEncode);
	static std::string StringTools::UrlDecode(const std::string& szToDecode);
};

#endif /* _FILETOOLS_HPP_ */