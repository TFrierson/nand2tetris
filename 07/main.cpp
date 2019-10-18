#include <iostream>
#include <string>
#include <fstream>
#include "Parser.h"
#include "CodeWriter.h"

int main(int argc, char* argv[])
{
	CodeWriter cw1;

	for (int i = 1; i < argc; i++)
	{
		std::string nameStr = argv[i];
		Parser parse(nameStr);
		cw1.setFileName(nameStr);

		while (parse.hasMoreCommands())
		{
			parse.advance();

			if (parse.commandType() == "C_ARITHMETIC")
			{
				cw1.writeArithmetic(parse.arg1());
			}

			else if (parse.commandType() == "C_PUSH")
			{
				cw1.writePushPop("push", parse.arg1(), parse.arg2());
			}

			else if (parse.commandType() == "C_POP")
			{
				cw1.writePushPop("pop", parse.arg1(), parse.arg2());
			}
		}
	}

	cw1.Close();

	return 0;
}
