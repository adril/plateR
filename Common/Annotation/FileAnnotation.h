#pragma once

#include <iostream>
#include <string>

class FileAnnotation {
public:
	std::string _codeFile;
	std::string _filePath;
public:
	FileAnnotation(std::string codeFile, std::string filePath);
	virtual ~FileAnnotation(void);
};

