#include "fileTools.hpp"
#include <iostream>
#include <fstream>

bool FileTools::writeStringToFile(std::string fileContent, std::string filePath) {
	std::ofstream myFile;

	myFile.open(filePath, std::ios::out | std::ios::app | std::ios::binary);
	if (myFile.is_open()) {
		myFile << fileContent.c_str();
		myFile.close();
		return true;
	}
	else
		return false;
}