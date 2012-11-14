#include "FileAnnotation.h"


FileAnnotation::FileAnnotation(std::string codeFile, std::string filePath)
	/*: _codeFile(codeFile), _filePath(_filePath)*/ {
	this->_codeFile = codeFile;
	this->_filePath = filePath;
}

FileAnnotation::~FileAnnotation(void) {
}
