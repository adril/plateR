#include "fileTools.hpp"
#include <iostream>
#include <fstream>

bool FileTools::writeStringToFile(std::string fileContent, std::string filePath) {
	std::ofstream myFile;

	FILE *file = fopen(filePath.c_str(), "wb");
	if (file) {
		fwrite(fileContent.c_str(), fileContent.length(), 1, file);
		fclose(file);
		return true;
	}
	return false;
	//std::ofstream myFile;

	//myFile.open(filePath, std::ios::out | std::ios::app | std::ios::binary);
	//if (myFile.is_open()) {
	//	myFile << fileContent.c_str();
	//	myFile.close();
	//	return true;
	//}
	//return false;
}

std::string FileTools::readStringFromFile(std::string filePath)
{
	unsigned int size = 0;
	char *memblock;

	std::ifstream file (filePath.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
	if (file.is_open())
	{
		size = (unsigned int)file.tellg();
		memblock = new char[size];
		file.seekg (0, std::ios::beg);
		file.read (memblock, size);
		file.close();
		std::string buffer(memblock, size);
		return buffer;
	}
	return NULL;
}