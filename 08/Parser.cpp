#include "Parser.h"
#include <iostream>
#include <string>
#include <fstream>

Parser::Parser()
{
	file = "";
}

void Parser::setFileName(std::string fileInput)
{
	file = fileInput;
	inputFile.open(file);

	if (!inputFile.is_open())
	{
		std::cerr << "Error, could not open file." << std::endl;
	}
}

bool Parser::hasMoreCommands()
{
	return !(inputFile.eof());
}

void Parser::advance()
{
	if (hasMoreCommands())
	{		
		std::getline(inputFile, currentCommand);
	}
}

std::string Parser::commandType()
{
	int spacePos = currentCommand.find(" ");

	if (currentCommand.substr(0,2) == "eq" || currentCommand.substr(0,2) == "lt" || 
		currentCommand.substr(0,2) == "gt" || currentCommand.substr(0,2) == "or" ||
		currentCommand.substr(0,3) == "and" || currentCommand.substr(0,3) == "not" ||
		currentCommand.substr(0,3) == "add" || currentCommand.substr(0,3) == "sub" ||
		currentCommand.substr(0,3) == "neg")
	{
		return "C_ARITHMETIC";
	}

	else
	{
		std::string commandTypeStr = currentCommand.substr(0, spacePos);

		if (commandTypeStr == "push")
		{
			return "C_PUSH";
		}

		else if (commandTypeStr == "pop")
		{
			return "C_POP";
		}

		else if (commandTypeStr == "label")
		{
			return "C_LABEL";
		}

		else if (commandTypeStr == "goto")
		{
			return "C_GOTO";
		}

		else if (commandTypeStr.substr(0, 2) == "if")
		{
			return "C_IF";
		}

		else if (commandTypeStr == "function")
		{
			return "C_FUNCTION";
		}

		else if (commandTypeStr == "return")
		{
			return "C_RETURN";
		}

		else if(commandTypeStr == "call")
		{
			return "C_CALL";
		}

		else
		{
			return "error";
		}
	}
}

std::string Parser::arg1()
{
	int space1 = currentCommand.find(" ");

	if (space1 == std::string::npos)
	{
		return currentCommand;
	}

	else if(commandType() == "C_ARITHMETIC"){
		return currentCommand.substr(0, space1);
	}

	else
	{
		int space2 = currentCommand.find(" ", space1 + 1);

		return currentCommand.substr(space1 + 1, space2 - space1 - 1);

	}
}

int Parser::arg2()
{
	int space1_1 = currentCommand.find(" ");
	int space2_1 = currentCommand.find(" ", space1_1 + 1);

	std::string index = currentCommand.substr(space2_1 + 1);

	return (std::stoi(index));
}

//Parser destructor
Parser::~Parser()
{
	if (inputFile.is_open())
	{
		inputFile.close();
	}
}
