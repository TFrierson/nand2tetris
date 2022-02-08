#pragma once
#include <iostream>
#include <string>
#include <fstream>

class CodeWriter
{
public:
	CodeWriter();
	void setFileName(std::string fileName);
	std::string outputFileName;
	std::string moduleName;
	std::ofstream outputFile;
	int eqLabelNum;
	int gtLabelNum;
	int ltLabelNum;
	void writeArithmetic(std::string command);
	void writePushPop(std::string cmmd, std::string segment, int index);
	void incrStack();
	void decrStack();
	void Close();

	~CodeWriter();
};

