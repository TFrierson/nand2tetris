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
	std::string fileName;
	std::string functionName;
	std::ofstream outputFile;
	int labelNum;
	int start;
	void writeInit();
	void writeArithmetic(std::string command);
	void writePushPop(std::string cmmd, std::string segment, int index);
	void writeLabel(std::string label);
	void writeGoTo(std::string label);
	void writeIf(std::string label);
	void writeCall(std::string funcName, int numArgs);
	void writeFunction(std::string functName, int numLocals);
	void setReturn(int input);
	void getReturn(int input);
	void writeReturn();
	void incrStack();
	void decrStack();
	void Close();

	~CodeWriter();
};

