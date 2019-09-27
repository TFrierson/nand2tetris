#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>

int main()
{
	short output;
	std::cout << "Enter asm file name. Include .asm" << std::endl;
	std::string progName;
	std::getline(std::cin, progName);

	std::fstream inputFile;
	inputFile.open(progName);

	int dot = progName.find('.');
	progName = progName.substr(0, dot);
	progName += ".hack";
	std::ofstream outputFile;
	outputFile.open(progName);
	
	int lineNum = 0b0000000000000000;
	std::vector<std::string> prog;
	std::string input;

	std::unordered_map<std::string, int> moduleStart;

	std::string varLabel = "";
	std::unordered_map<std::string, int> variables;

	//variable memory starts at 16
	int varMemNum = 0b0000000000010000;

	std::unordered_map<std::string, int> destination;
	std::unordered_map<std::string, int> aComp;
	std::unordered_map<std::string, int> mComp;
	std::unordered_map<std::string, int> jump;

	destination.insert(std::make_pair("null", 0b0000000000000000));
	destination.insert(std::make_pair("M", 0b0000000000001000));
	destination.insert(std::make_pair("D", 0b0000000000010000));
	destination.insert(std::make_pair("A", 0b0000000000100000));
	destination.insert(std::make_pair("MD", 0b0000000000011000));
	destination.insert(std::make_pair("AM", 0b0000000000101000));
	destination.insert(std::make_pair("AD", 0b0000000000110000));
	destination.insert(std::make_pair("AMD", 0b0000000000111000));

	jump.insert(std::make_pair("null", 0b0000000000000000));
	jump.insert(std::make_pair("JGT", 0b0000000000000001));
	jump.insert(std::make_pair("JEQ", 0b0000000000000010));
	jump.insert(std::make_pair("JGE", 0b0000000000000011));
	jump.insert(std::make_pair("JLT", 0b0000000000000100));
	jump.insert(std::make_pair("JNE", 0b0000000000000101));
	jump.insert(std::make_pair("JLE", 0b0000000000000110));
	jump.insert(std::make_pair("JMP", 0b0000000000000111));

	variables.insert(std::make_pair("SP", 0b0000000000000000));
	variables.insert(std::make_pair("LCL", 0b0000000000000001));
	variables.insert(std::make_pair("ARG", 0b0000000000000010));
	variables.insert(std::make_pair("THIS", 0b0000000000000011));
	variables.insert(std::make_pair("THAT", 0b0000000000000100));
	variables.insert(std::make_pair("SCREEN", 0b0100000000000000));
	variables.insert(std::make_pair("KBD", 0b0110000000000000));
	variables.insert(std::make_pair("R0", 0b0000000000000000));
	variables.insert(std::make_pair("R1", 0b0000000000000001));
	variables.insert(std::make_pair("R2", 0b0000000000000010));
	variables.insert(std::make_pair("R3", 0b0000000000000011));
	variables.insert(std::make_pair("R4", 0b0000000000000100));
	variables.insert(std::make_pair("R5", 0b0000000000000101));
	variables.insert(std::make_pair("R6", 0b0000000000000110));
	variables.insert(std::make_pair("R7", 0b0000000000000111));
	variables.insert(std::make_pair("R8", 0b0000000000001000));
	variables.insert(std::make_pair("R9", 0b0000000000001001));
	variables.insert(std::make_pair("R10", 0b0000000000001010));
	variables.insert(std::make_pair("R11", 0b0000000000001011));
	variables.insert(std::make_pair("R12", 0b0000000000001100));
	variables.insert(std::make_pair("R13", 0b0000000000001101));
	variables.insert(std::make_pair("R14", 0b0000000000001110));
	variables.insert(std::make_pair("R15", 0b0000000000001111));

	aComp.insert(std::make_pair("0", 0b0000101010000000));
	aComp.insert(std::make_pair("1", 0b0000111111000000));
	aComp.insert(std::make_pair("-1", 0b0000111010000000));
	aComp.insert(std::make_pair("D", 0b0000001100000000));
	aComp.insert(std::make_pair("A", 0b0000110000000000));
	aComp.insert(std::make_pair("!D", 0b0000001101000000));
	aComp.insert(std::make_pair("!A", 0b0000110001000000));
	aComp.insert(std::make_pair("-D", 0b0000001111000000));
	aComp.insert(std::make_pair("-A", 0b0000110011000000));
	aComp.insert(std::make_pair("D+1", 0b0000011111000000));
	aComp.insert(std::make_pair("A+1", 0b0000110111000000));
	aComp.insert(std::make_pair("D-1", 0b0000001110000000));
	aComp.insert(std::make_pair("A-1", 0b0000110010000000));
	aComp.insert(std::make_pair("D+A", 0b0000000010000000));
	aComp.insert(std::make_pair("D-A", 0b0000010011000000));
	aComp.insert(std::make_pair("A-D", 0b0000000111000000));
	aComp.insert(std::make_pair("D&A", 0b0000000000000000));
	aComp.insert(std::make_pair("D|A", 0b0000010101000000));

	mComp.insert(std::make_pair("M", 0b0000110000000000));
	mComp.insert(std::make_pair("!M", 0b0000110001000000));
	mComp.insert(std::make_pair("-M", 0b0000110011000000));
	mComp.insert(std::make_pair("M+1", 0b0000110111000000));
	mComp.insert(std::make_pair("M-1", 0b0000110010000000));
	mComp.insert(std::make_pair("D+M", 0b0000000010000000));
	mComp.insert(std::make_pair("D-M", 0b0000010011000000));
	mComp.insert(std::make_pair("M-D", 0b0000000111000000));
	mComp.insert(std::make_pair("D&M", 0b0000000000000000));
	mComp.insert(std::make_pair("D|M", 0b0000010101000000));

	if (inputFile.is_open())
	{
		std::string line;

		while (std::getline(inputFile, line))
		{
			if (line.length() > 0)
			{
				if (line[0] == '/')
				{
					continue;
				}

				else
				{
					//We'll take care of the in-line comments and whitespaces here
					int a = 0;

					while (a < line.length())
					{
						if (line[a] == ' ')
						{
							line.erase(line.begin() + a);
						}

						else
						{
							a++;
						}
					}

					int commentPos = line.find('//');

					if (commentPos != std::string::npos)
					{
						line = line.substr(0, commentPos);
					}

					//Program module parsing starts here
					if (line[0] == '(')
					{
						std::string moduleLabel = line.substr(1, line.length() - 2);
						moduleStart.insert(std::make_pair(moduleLabel, lineNum));
					}

					else
					{
						prog.push_back(line);
						lineNum++;
					}
				}
			}
		}
	}

	inputFile.close();

	for (int j = 0; j < prog.size(); j++)
	{
		//if the first char in the prog line is an @, we will look in variables & moduleStart. If in neither, then 
		//it will go in variables, since moduleStart was filled in the first part of the program
		if (prog[j][0] == '@')
		{
			std::string lineString = prog[j].substr(1, prog[j].length() - 1);

			//if found in moduleStart, then copy the prog line in moduleStart to the output
			if (moduleStart.find(lineString) != moduleStart.end())
			{
				output = moduleStart[lineString];
			}

			//if NOT found in moduleStart
			else
			{
				//but IS found in variables....
				if (variables.find(lineString) != variables.end())
				{
					output = variables[lineString];
				}

				//else, it's not found in moduleStart OR variables, so put into variables
				else
				{
					if (isdigit(lineString[0]))
					{
						int varNum = stoi(lineString);
						variables.insert(std::make_pair(lineString, varNum));
						output = varNum;
					}

					else
					{
						variables.insert(std::make_pair(lineString, varMemNum));
						output = varMemNum;
						varMemNum++;
					}
				}
			}
		}

		//if there is NO @, then it is a comp line. Proceed accordingly
		else
		{
			int scPos = prog[j].find(';');

			//now, it's either a jump statement or a comp statement

			//if the line has a semicolon, then it is a jump statement
			if (scPos != std::string::npos)
			{
				std::string destString = prog[j].substr(0, scPos);
				std::string jumpStr = prog[j].substr(scPos + 1);

				output = 0b1110000000000000;
				output += jump[jumpStr];

				if (aComp.find(destString) != aComp.end())
				{
					output += aComp[destString];
				}

				else
				{
					output += mComp[destString];
				}
			}

			//if not, then it is a comp statement
			else
			{
				int eqPos = prog[j].find('=');
				std::string destString = prog[j].substr(0, eqPos);
				std::string compString = prog[j].substr(eqPos + 1);

				if (aComp.find(compString) != aComp.end())
				{
					output = 0b1110000000000000;
					output += aComp[compString];
				}

				else
				{
					output = 0b1111000000000000;
					output += mComp[compString];
				}

				output += destination[destString];
			}
		}
				
		//at the end of each line, we will output 'output' to the output file
		std::bitset<16> outputNumBin(output);
		outputFile << outputNumBin << std::endl;
	}

	return 0;
}