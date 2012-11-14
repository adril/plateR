#pragma once

#include <iostream>
#include <string>

class PlateAnnotation {
public:
	std::string _codeFile;
	char _state;
public:
	PlateAnnotation(std::string codeFile, char state);
	virtual ~PlateAnnotation(void);
};

