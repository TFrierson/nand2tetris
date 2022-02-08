#include "CodeWriter.h"
#include <fstream>
#include <string>
#include <iostream>


//study up on pointer and static
CodeWriter::CodeWriter()
{
	int eqLabelNum = 0;
	int gtLabelNum = 0;
	int ltLabelNum = 0;
}

void CodeWriter::setFileName(std::string fileName)
{
	outputFileName = fileName;
	int dotPos = outputFileName.find(".");
	outputFileName = outputFileName.substr(0, dotPos);
	moduleName = outputFileName;
	outputFileName += ".asm";

	outputFile.open(outputFileName);
}

void CodeWriter::incrStack()
{
	outputFile << "@SP" << std::endl;
	outputFile << "AM=M+1" << std::endl;
}

void CodeWriter::decrStack()
{
	outputFile << "@SP" << std::endl;
	outputFile << "AM=M-1" << std::endl;
}

void CodeWriter::writeArithmetic(std::string command)
{
	if (command == "add")
	{
		//took out the last push and many points to the stack
		decrStack();
		outputFile << "D=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=D+M" << std::endl;
	}

	else if (command == "sub")
	{
		decrStack();
		outputFile << "D=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=M-D" << std::endl;
	}

	else if (command == "neg")
	{
		outputFile << "@SP" << std::endl;
		//leaving A=M out gives A=-1
		outputFile << "A=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=-M" << std::endl;
	}

	else if (command == "eq")
	{
		std::string eqLabelFalse = "EQFALSE" + std::to_string(eqLabelNum);
		std::string eqLabelEnd = "EQEND" + std::to_string(eqLabelNum);
		eqLabelNum++;

		//Use full @SP AM=M-1 when decrementing twice
		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "D=M-D" << std::endl;
		outputFile << "@" << eqLabelFalse << std::endl;
		outputFile << "D;JEQ" << std::endl;
		outputFile << "D=0" << std::endl;
		outputFile << "@" << eqLabelEnd << std::endl;
		outputFile << "0;JMP" << std::endl;
		outputFile << "(" << eqLabelFalse << ")" << std::endl;
		outputFile << "D=-1" << std::endl;
		outputFile << "(" << eqLabelEnd << ")" << std::endl;
		outputFile << "@SP" << std::endl;
		outputFile << "A=M" << std::endl;
		outputFile << "M=D" << std::endl;
		incrStack();
	}

	else if (command == "gt")
	{
		std::string gtLabelTrue = "GTTRUE" + std::to_string(gtLabelNum);
		std::string gtLabelEnd = "GTEND" + std::to_string(gtLabelNum);
		gtLabelNum++;

		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "D=M-D" << std::endl;
		outputFile << "@" << gtLabelTrue << std::endl;
		outputFile << "D;JGT" << std::endl;
		outputFile << "D=0" << std::endl;
		outputFile << "@" << gtLabelEnd << std::endl;
		outputFile << "0;JMP" << std::endl;
		outputFile << "(" << gtLabelTrue << ")" << std::endl;
		outputFile << "D=-1" << std::endl;
		outputFile << "@" << gtLabelEnd << std::endl;
		outputFile << "0;JMP" << std::endl;
		outputFile << "(" << gtLabelEnd << ")" << std::endl;
		outputFile << "@SP" << std::endl;
		outputFile << "A=M" << std::endl;
		outputFile << "M=D" << std::endl;
		incrStack();
	}

	else if (command == "lt")
	{
		std::string ltLabelTrue = "LTTRUE" + std::to_string(ltLabelNum);
		std::string ltLabelEnd = "LTEND" + std::to_string(ltLabelNum);
		ltLabelNum++;

		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "D=M-D" << std::endl;
		outputFile << "@" << ltLabelTrue << std::endl;
		outputFile << "D;JLT" << std::endl;
		outputFile << "D=0" << std::endl;
		outputFile << "@" << ltLabelEnd << std::endl;
		outputFile << "0;JMP" << std::endl;
		outputFile << "(" << ltLabelTrue << ")" << std::endl;
		outputFile << "D=-1" << std::endl;
		outputFile << "(" << ltLabelEnd << ")" << std::endl;
		outputFile << "@SP" << std::endl;
		outputFile << "A=M" << std::endl;
		outputFile << "M=D" << std::endl;
		incrStack();
	}

	else if (command == "and")
	{
		decrStack();
		outputFile << "D=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=D&M" << std::endl;
	}

	else if (command == "or")
	{
		decrStack();
		outputFile << "D=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=D|M" << std::endl;
	}

	else if (command == "not")
	{
		outputFile << "@SP" << std::endl;
		outputFile << "A=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=!M" << std::endl;	
	}
}

void CodeWriter::writePushPop(std::string cmmd, std::string segment, int index)
{
	if (cmmd == "push")
	{
		//command is: take data from segment index and push it onto the stack
		if (segment == "constant")
		{
			outputFile << "@" << index << std::endl;
			outputFile << "D=A" << std::endl;
		}

		else if (segment == "local")
		{
			
			outputFile << "@LCL" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "A=D+A" << std::endl;
			outputFile << "D=M" << std::endl;
		}

		else if (segment == "argument")
		{
			outputFile << "@ARG" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "A=D+A" << std::endl;
			outputFile << "D=M" << std::endl;
		}

		else if (segment == "pointer")
		{
			index += 3;
			outputFile << "@R" << index << std::endl;
			outputFile << "D=M" << std::endl;
		}

		else if (segment == "this")
		{
			
			outputFile << "@THIS" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "A=D+A" << std::endl;
			outputFile << "D=M" << std::endl;
		}

		else if (segment == "that")
		{
			outputFile << "@THAT" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "A=D+A" << std::endl;
			outputFile << "D=M" << std::endl;
		}

		else if (segment == "temp")
		{
			index += 5;
			outputFile << "@R" << index << std::endl;
			outputFile << "D=M" << std::endl;
		}

		else
		{
			//else it's static
			outputFile << "@" << moduleName << "." << index << std::endl;
			outputFile << "D=M" << std::endl;
		}

		outputFile << "@SP" << std::endl;
		outputFile << "A=M" << std::endl;
		outputFile << "M=D" << std::endl;
		incrStack();
	}

	else if (cmmd == "pop")
	{
		//command is: take from the stack and put it in segment index. Algorithm is save the segment + index in R13
		//head to the stack, put the data into M, point A to the address stored in R13 then put the data into that
		if (segment == "local")
		{
			outputFile << "@LCL" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "D=D+A" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "M=D" << std::endl;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "A=M" << std::endl;
		}

		else if (segment == "argument")
		{
			outputFile << "@ARG" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "D=D+A" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "M=D" << std::endl;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "A=M" << std::endl;
		}

		else if (segment == "this")
		{
			outputFile << "@THIS" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "D=D+A" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "M=D" << std::endl;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "A=M" << std::endl;
		}

		else if (segment == "that")
		{
			outputFile << "@THAT" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "D=D+A" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "M=D" << std::endl;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "A=M" << std::endl;
		}

		else if (segment == "pointer")
		{
			index += 3;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R" << index << std::endl;
		}

		else if (segment == "temp")
		{
			index += 5;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R" << index << std::endl;
		}

		else
		{
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@" << moduleName << "." << index << std::endl;
		}

		outputFile << "M=D" << std::endl;
	}
}

void CodeWriter::Close()
{
	if (outputFile.is_open())
	{
		outputFile.close();
	}
}

CodeWriter::~CodeWriter()
{
	if (outputFile.is_open())
	{
		outputFile.close();
	}
}
