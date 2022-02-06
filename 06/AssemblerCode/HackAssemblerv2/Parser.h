#pragma once
#include <iostream>
#include <bitset>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

class Parser{
public:
	Parser(std::string fileInput);
	~Parser();
	bool hasMoreLines();
	void advance();
	std::string instructionType(std::string input);
    std::string symbol(std::string input);
    std::string dest(std::string input);
    std::string comp(std::string input);
    std::string jump(std::string input);

	std::string currentInstruction;
	std::vector<std::string> editedCommands;

private:
	std::string file;
	std::ifstream inputFile;
};