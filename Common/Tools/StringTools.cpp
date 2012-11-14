#include "StringTools.hpp"
#include <iostream>
#include <fstream>

void StringTools::copyString(char *src, char *dest, size_t size) {
	std::memset(dest, '\0', size);
	std::memcpy(dest, src, size);
}