#include "CodeWriter.h"
#include <fstream>
#include <string>
#include <cassert>
#include <iostream>

CodeWriter::CodeWriter()
{
	int labelNum = 0;
	int start = 0;
}

void CodeWriter::setFileName(std::string fName)
{
	int dotPos = fName.find('.');
	fName = fName.substr(0, dotPos);

	if (start == 0)
	{
		outputFileName = fName;
		outputFileName += ".asm";

		outputFile.open(outputFileName);
		writeInit();
	}

	else
	{
		fileName = fName;
	}
}

void CodeWriter::writeInit()
{
	outputFile << "@256" << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "M=D" << std::endl;
	outputFile << "@START" << start << std::endl;
	outputFile << "0;JMP" << std::endl;

	//Maketrue decreases the length of the output file
	outputFile << "(MAKETRUE)" << std::endl;
	decrStack();
	//True = -1, false = 0
	outputFile << "M=-1" << std::endl;
	incrStack();
	getReturn(15);
	outputFile << "(START" << start << ")" << std::endl;
	writeCall("Sys.init", 0);
	start++;	
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

void CodeWriter::writeLabel(std::string label)
{
	outputFile << "(" << functionName << "$" << label << ")" << std::endl;
}

void CodeWriter::writeArithmetic(std::string command)
{
	if (command == "add")
	{
		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "M=D+M" << std::endl;
		incrStack();
	}

	else if (command == "sub")
	{
		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "M=M-D" << std::endl;
		incrStack();
	}

	else if (command == "neg")
	{
		outputFile << "@SP" << std::endl;
		//leaving A=M out gives A=-1
		outputFile << "A=M" << std::endl;
		outputFile << "AM=M-1" << std::endl;
		outputFile << "M=-M" << std::endl;
		incrStack();
	}

	else if (command == "eq")
	{
		setReturn(15);
		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "D=M-D" << std::endl;
		outputFile << "M=0" << std::endl;
		incrStack();
		outputFile << "@MAKETRUE" << std::endl;
		outputFile << "D;JEQ" << std::endl;
		outputFile << "(RETURN" << labelNum << ")" << std::endl;
		labelNum++;
	}

	else if (command == "gt")
	{
		setReturn(15);
		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "D=M-D" << std::endl;
		outputFile << "M=0" << std::endl;
		incrStack();
		outputFile << "@MAKETRUE" << std::endl;
		outputFile << "D;JGT" << std::endl;
		outputFile << "(RETURN" << labelNum << ")" << std::endl;
		labelNum++;
	}

	else if (command == "lt")
	{
		setReturn(15);
		decrStack();
		outputFile << "D=M" << std::endl;
		decrStack();
		outputFile << "D=M-D" << std::endl;
		outputFile << "M=0" << std::endl;
		incrStack();
		outputFile << "@MAKETRUE" << std::endl;
		outputFile << "D;JLT" << std::endl;
		outputFile << "(RETURN" << labelNum << ")" << std::endl;
		labelNum++;
	}

	else if (command == "and")
	{
		decrStack();
		outputFile << "D=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=D&M" << std::endl;
		incrStack();
	}

	else if (command == "or")
	{
		decrStack();
		outputFile << "D=M" << std::endl;
		outputFile << "A=A-1" << std::endl;
		outputFile << "M=D|M" << std::endl;
		incrStack();
	}

	else if (command == "not")
	{
		outputFile << "@SP" << std::endl;
		outputFile << "AM=M-1" << std::endl;
		outputFile << "M=!M" << std::endl;
		incrStack();
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
			outputFile << "@3" << std::endl;
			outputFile << "D=A" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "A=D+A" << std::endl;
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
			outputFile << "@5" << std::endl;
			outputFile << "D=A" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "A=D+A" << std::endl;
			outputFile << "D=M" << std::endl;
		}

		else
		{
			//else it's static
			outputFile << "@" << fileName << "." << index << std::endl;
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
			outputFile << "@3" << std::endl;
			outputFile << "D=A" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "D=D+A" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "M=D" << std::endl;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "A=M" << std::endl;
			outputFile << "M=D" << std::endl;
		}

		else if (segment == "temp")
		{
			outputFile << "@5" << std::endl;
			outputFile << "D=A" << std::endl;
			outputFile << "@" << index << std::endl;
			outputFile << "D=D+A" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "M=D" << std::endl;
			decrStack();
			outputFile << "D=M" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "A=M" << std::endl;
			outputFile << "M=D" << std::endl;
		}

		else
		{
			outputFile << "@" << fileName << "." << index << std::endl;
			outputFile << "D=A" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "M=D" << std::endl;
			outputFile << "@SP" << std::endl;
			outputFile << "M=M-1" << std::endl;
			outputFile << "A=M" << std::endl;
			outputFile << "D=M" << std::endl;
			outputFile << "@R13" << std::endl;
			outputFile << "A=M" << std::endl;
		}

		outputFile << "M=D" << std::endl;
	}
}

void CodeWriter::writeGoTo(std::string label)
{
	if (label != functionName)
	{
		outputFile << "@" << functionName << "$" << label << std::endl;
		outputFile << "0;JMP" << std::endl;
	}

	else
	{
		outputFile << "@" << label << std::endl;
		outputFile << "0;JMP" << std::endl;
	}
}

void CodeWriter::writeIf(std::string label)
{
	outputFile << "@SP" << std::endl;
	outputFile << "AM=M-1" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@" << functionName << "$" << label << std::endl;
	outputFile << "D;JNE" << std::endl;
}

void CodeWriter::writeCall(std::string funcName, int numArgs)
{		
	//push return-address
	outputFile << "@RETURN" << labelNum << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "M=D" << std::endl;
	incrStack();

	//push LCL
	outputFile << "@LCL" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "M=D" << std::endl;
	incrStack();

	//push ARG
	outputFile << "@ARG" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "M=D" << std::endl;
	incrStack();

	//push THIS
	outputFile << "@THIS" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "M=D" << std::endl;
	incrStack();

	//push THAT
	outputFile << "@THAT" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "M=D" << std::endl;
	incrStack();

	//ARG = SP - n - 5
	outputFile << "@" << numArgs << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "D=M-D" << std::endl;
	outputFile << "@5" << std::endl;
	outputFile << "D=D-A" << std::endl;
	outputFile << "@ARG" << std::endl;
	outputFile << "M=D" << std::endl;

	//LCL = SP
	outputFile << "@SP" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@LCL" << std::endl;
	outputFile << "M=D" << std::endl;

	//goto f
	outputFile << "@" << funcName << std::endl;
	outputFile << "0;JMP" << std::endl;

	//(return-address)
	outputFile << "(RETURN" << labelNum << ")" << std::endl;
	labelNum++;
}

void CodeWriter::writeFunction(std::string functName, int numLocals)
{
	functionName = functName;

	outputFile << "(" << functName << ")" << std::endl;

	for (int i = 0; i < numLocals; i++)
	{
		writePushPop("push", "constant", 0);
	}
}

void CodeWriter::writeReturn()
{
	//Frame = LCL
	outputFile << "@LCL" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@FRAME" << std::endl;
	outputFile << "M=D" << std::endl;

	//RET = *(FRAME - 5)
	outputFile << "@5" << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@FRAME" << std::endl;
	outputFile << "A=M-D" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@RET" << std::endl;
	outputFile << "M=D" << std::endl;

	//*ARG = pop()
	outputFile << "@SP" << std::endl;
	outputFile << "AM=M-1" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@ARG" << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "M=D" << std::endl;

	//SP = ARG + 1
	outputFile << "@ARG" << std::endl;
	outputFile << "D=M+1" << std::endl;
	outputFile << "@SP" << std::endl;
	outputFile << "M=D" << std::endl;
	
	//THAT = *(FRAME - 1)
	outputFile << "@FRAME" << std::endl;
	outputFile << "A=M-1" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@THAT" << std::endl;
	outputFile << "M=D" << std::endl;
	
	//THIS = *(FRAME - 2)
	outputFile << "@2" << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@FRAME" << std::endl;
	outputFile << "A=M-D" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@THIS" << std::endl;
	outputFile << "M=D" << std::endl;

	//ARG = *(FRAME - 3)
	outputFile << "@3" << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@FRAME" << std::endl;
	outputFile << "A=M-D" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@ARG" << std::endl;
	outputFile << "M=D" << std::endl;

	//LCL = *(FRAME - 4)
	outputFile << "@4" << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@FRAME" << std::endl;
	outputFile << "A=M-D" << std::endl;
	outputFile << "D=M" << std::endl;
	outputFile << "@LCL" << std::endl;
	outputFile << "M=D" << std::endl;

	//goto RET
	outputFile << "@RET" << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "0;JMP" << std::endl;
}

void CodeWriter::setReturn(int regNum)
{
	assert(13 <= regNum && regNum <= 15);
	outputFile << "@RETURN" << labelNum << std::endl;
	outputFile << "D=A" << std::endl;
	outputFile << "@R" << regNum << std::endl;
	outputFile << "M=D" << std::endl;
}

void CodeWriter::getReturn(int regNum)
{
	assert(13 <= regNum && regNum <= 15);
	outputFile << "@R" << regNum << std::endl;
	outputFile << "A=M" << std::endl;
	outputFile << "0;JMP" << std::endl;
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
