#include <iostream>
#include <string>
#include <fstream>
#include "Parser.h"
#include "CodeWriter.h"

int main(int argc, char* argv[])
{
	if(argc == 0)
	{
		std::cout << "Usage: VMTranslator file1.vm file2.vm ..." << std::endl;
	}

	else
	{
		CodeWriter cw1;

		for (int i = 1; i < argc; i++)
		{
			std::string nameStr = argv[i];
			Parser parse;
			parse.setFileName(nameStr);
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

				else if (parse.commandType() == "C_LABEL")
				{
					cw1.writeLabel(parse.arg1());
				}

				else if (parse.commandType() == "C_GOTO")
				{
					cw1.writeGoTo(parse.arg1());
				}

				else if (parse.commandType() == "C_IF")
				{
					cw1.writeIf(parse.arg1());
				}

				else if (parse.commandType() == "C_CALL")
				{
					cw1.writeCall(parse.arg1(), parse.arg2());
				}

				else if (parse.commandType() == "C_FUNCTION")
				{
					cw1.writeFunction(parse.arg1(), parse.arg2());
				}

				else if (parse.commandType() == "C_RETURN")
				{
					cw1.writeReturn();
				}
			}
		}

	cw1.Close();
	}

	return 0;
}
