#pragma once
#include <iostream>
#include <string>
#include <fstream>

class Parser
{
public:
	Parser();
	void setFileName(std::string fileInput);
	~Parser();
	bool hasMoreCommands();
	void advance();
	std::string commandType();
	std::string arg1();
	int arg2();

	std::string file;
	std::ifstream inputFile;
	std::string currentCommand;
};

