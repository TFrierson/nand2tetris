#include <iostream>
#include <bitset>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Parser.h"
#include "CodeModule.h"

//I need to run dos2unix on each .asm file to make sure this
//assembler parses the files correctly.

//In order to make the parser work correctly when there are in-line comments and space, I need to
//create a vector to store the edited strings (the strings with the comments and whitespace taken out)
//Then run through that vector for the second run through
int main(){
    std::unordered_map<std::string, std::string>symbolTable;
	int memoryNumber = 16;
	int lineNumber = 0;

    symbolTable.insert(std::make_pair("R0", "0000000000000000"));
    symbolTable.insert(std::make_pair("R1", "0000000000000001"));
    symbolTable.insert(std::make_pair("R2", "0000000000000010"));
    symbolTable.insert(std::make_pair("R3", "0000000000000011"));
    symbolTable.insert(std::make_pair("R4", "0000000000000100"));
    symbolTable.insert(std::make_pair("R5", "0000000000000101"));
    symbolTable.insert(std::make_pair("R6", "0000000000000110"));
    symbolTable.insert(std::make_pair("R7", "0000000000000111"));
    symbolTable.insert(std::make_pair("R8", "0000000000001000"));
    symbolTable.insert(std::make_pair("R9", "0000000000001001"));
    symbolTable.insert(std::make_pair("R10", "0000000000001010"));
    symbolTable.insert(std::make_pair("R11", "0000000000001011"));
    symbolTable.insert(std::make_pair("R12", "0000000000001100"));
    symbolTable.insert(std::make_pair("R13", "0000000000001101"));
    symbolTable.insert(std::make_pair("R14", "0000000000001110"));
    symbolTable.insert(std::make_pair("R15", "0000000000001111"));
    symbolTable.insert(std::make_pair("SP", "0000000000000000"));
    symbolTable.insert(std::make_pair("LCL", "0000000000000001"));
    symbolTable.insert(std::make_pair("ARG", "0000000000000010"));
    symbolTable.insert(std::make_pair("THIS", "0000000000000011"));
    symbolTable.insert(std::make_pair("THAT", "0000000000000100"));
    symbolTable.insert(std::make_pair("SCREEN", "0100000000000000"));
    symbolTable.insert(std::make_pair("KBD", "0110000000000000"));

    //Get the file's name and open it
    std::fstream inputFile;
    std::ofstream outputFile;
    std::string progName;
    std::string outputfileName;
    std::cout << "Enter the name of the program, include the .asm" << std::endl;
    std::getline(std::cin, progName);

    //first run. We're just creating symbols and populating Parse's editedCommands table
    Parser parse(progName);
    while(parse.hasMoreLines()){
        parse.advance();
    }

    for(int a = 0; a < parse.editedCommands.size(); a++){
        if(parse.instructionType(parse.editedCommands[a]) == "C_INSTRUCTION" 
            || parse.instructionType(parse.editedCommands[a]) == "A_INSTRUCTION"){
                lineNumber++;
            }

        if(parse.instructionType(parse.editedCommands[a]) == "L_INSTRUCTION"){
            if(symbolTable.find(parse.symbol(parse.editedCommands[a])) == symbolTable.end()){
                std::string lineNumberString = std::bitset<16>(lineNumber).to_string();
                symbolTable.insert(std::make_pair(parse.symbol(parse.editedCommands[a]), lineNumberString));
            }
        } 
    }

    //Create the output file and open it
    int dotPosition = progName.find('.');
    outputfileName = progName.substr(0, dotPosition) + ".hack";
    outputFile.open(outputfileName);

    if(!outputFile.is_open()){
        std::cout << "Error opening " << outputfileName << std::endl;
        return 1;
    }

    CodeModule cm;

    //Second run creating the code
    for(int i = 0; i < parse.editedCommands.size(); i++){
        
        if(parse.instructionType(parse.editedCommands[i]) == "C_INSTRUCTION"){
            outputFile << cm.comp(parse.comp(parse.editedCommands[i])) << 
                cm.dest(parse.dest(parse.editedCommands[i])) << cm.jump(parse.jump(parse.editedCommands[i])) << std::endl;
        }

        if(parse.instructionType(parse.editedCommands[i]) == "A_INSTRUCTION"){
            //If the current instruction starts with a digit, then it is a constant
            if(isdigit(parse.editedCommands[i][1])){
                outputFile << parse.symbol(parse.editedCommands[i]) << std::endl;
            }

            else if(symbolTable.find(parse.symbol(parse.editedCommands[i])) == symbolTable.end()){
                std::string memoryNumberString = std::bitset<16>(memoryNumber).to_string();
                symbolTable.insert(std::make_pair(parse.symbol(parse.editedCommands[i]), memoryNumberString));
                memoryNumber++;

                outputFile << symbolTable[parse.symbol(parse.editedCommands[i])] << std::endl;
            }

            else{
                outputFile << symbolTable[parse.symbol(parse.editedCommands[i])] << std::endl;
            }
        }
    }

    outputFile.close();

    return 0;
}