#include "Parser.h"
#include <iostream>
#include <bitset>
#include <string>
#include <fstream>

Parser::Parser (std::string fileInput){
    file = fileInput;
    inputFile.open(file);
}

bool Parser::hasMoreLines(){
    return(!inputFile.eof());
}

void Parser::advance(){
    while(hasMoreLines()){
        std::getline(inputFile, currentInstruction);

        if(currentInstruction[0] == '/' || currentInstruction.length() == 0){
            continue;
        }

        int a = 0;

		while (a < currentInstruction.length()){
			if (currentInstruction[a] == ' '){
				currentInstruction.erase(currentInstruction.begin() + a);
			}

			else{
				a++;
			}
		}

		int commentPos = currentInstruction.find('/');

		if (commentPos != std::string::npos){
			currentInstruction = currentInstruction.substr(0, commentPos);
        }

        editedCommands.push_back(currentInstruction);
    }
}

std::string Parser::instructionType(std::string input){
    if(input.length() == 0){
        return "null";
    }

    else{
        if(input[0] == '@'){
            return("A_INSTRUCTION");
        }

        else if(input[0] == '('){
            return("L_INSTRUCTION");
        }

        else{
            return("C_INSTRUCTION");
        }
    }
}

std::string Parser::symbol(std::string input){
    if(input[0] == '('){
        int closingParenthesis = input.find(')');
        return(input.substr(1, closingParenthesis - 1));
    }

    else{
        if(isdigit(input[1])){
            int numberString = std::stoi(input.substr(1));
            return(std::bitset<16>(numberString).to_string());
        }

        else{
            return(input.substr(1));
        }
    }
}

std::string Parser::dest(std::string input){
    int equalPosition = input.find('=');
    int semicolonPosition = input.find(';');

    if(semicolonPosition == std::string::npos){
        return(input.substr(0, equalPosition));
    }

    else{
        return("null");
    }
}

std::string Parser::comp(std::string input){
    int semicolonPosition = input.find(';');
    int equalPosition = input.find('=');

    if(semicolonPosition == std::string::npos){
        return(input.substr(equalPosition + 1));
    }

    else{
        return(input.substr(0, semicolonPosition));
    }
}

std::string Parser::jump(std::string input){
    int semicolonPosition = input.find(';');
    int equalPosition = input.find('=');


    if(semicolonPosition == std::string::npos){
        return("null");
    }

    else{
        return(input.substr(semicolonPosition + 1));
    }
}

Parser::~Parser(){
    if(inputFile.is_open()){
        inputFile.close();
    }
}