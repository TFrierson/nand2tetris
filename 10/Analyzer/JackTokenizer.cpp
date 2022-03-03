#include "JackTokenizer.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>

JackTokenizer::JackTokenizer(std::string fileInput){
    inputFile.open(fileInput);

    if(!inputFile.is_open()){
        std::cerr << "Error opening " << fileInput << "." << std::endl;
        return;
    }

    else{
        keywords.insert("class");
        keywords.insert("constructor");
        keywords.insert("function");
        keywords.insert("method");
        keywords.insert("field");
        keywords.insert("static");
        keywords.insert("var");
        keywords.insert("int");
        keywords.insert("char");
        keywords.insert("boolean");
        keywords.insert("void");
        keywords.insert("true");
        keywords.insert("false");
        keywords.insert("null");
        keywords.insert("this");
        keywords.insert("let");
        keywords.insert("do");
        keywords.insert("if");
        keywords.insert("else");
        keywords.insert("while");
        keywords.insert("return");

        symbols.insert('{');
        symbols.insert('}');
        symbols.insert('(');
        symbols.insert(')');
        symbols.insert('[');
        symbols.insert(']');
        symbols.insert('.');
        symbols.insert(',');
        symbols.insert(';');
        symbols.insert('+');
        symbols.insert('-');
        symbols.insert('*');
        symbols.insert('/');
        symbols.insert('&');
        symbols.insert('|');
        symbols.insert('<');
        symbols.insert('>');
        symbols.insert('=');
        symbols.insert('~');
    }
}

bool JackTokenizer::hasMoreTokens(){
    return(!inputFile.eof());
}

void JackTokenizer::advance(){
    while(hasMoreTokens()){
        getline(inputFile, currentLine);

        if((currentLine.find("/*") != std::string::npos || 
                currentLine.find("/**") != std::string::npos) && hasMoreTokens()){
                int endComment = currentLine.find("*/");

                while(endComment == std::string::npos && hasMoreTokens()){
                    getline(inputFile, currentLine);
                    endComment = currentLine.find("*/");
                }

                continue;
            }

        std::stringstream tokenStream(currentLine);

        while(tokenStream){
            currentToken.clear();

            tokenStream >> currentToken;

            //Take care of comments
            if(currentToken.substr(0,2) == "//"){
                break;
            }

            if((currentToken.find("/*") != std::string::npos || 
                currentToken.find("/**") != std::string::npos) && hasMoreTokens()){
                int endComment = currentLine.find("*/");

                while(endComment == std::string::npos && hasMoreTokens()){
                    getline(inputFile, currentLine);
                }

                break;
            }

            //Take care of string literals here
            int quotePosition = currentToken.find("\"");
            if(quotePosition == std::string::npos && currentToken.length() != 0){
                extractSymbols(currentToken);
            }

            if(quotePosition != std::string::npos){
                if(quotePosition == 0){
                    std::string stringLiteral = currentToken;
                    stringLiteral += " ";
                    while(tokenStream){
                        currentToken.clear();
                        tokenStream >> currentToken;
                        stringLiteral += currentToken;
                    }

                    stringLiteral = currentToken.substr(0, currentToken.length() - 1);
                    currentToken = stringLiteral;
                    extractSymbols(currentToken);
                }
                
                else{
                    std::string tempToken = currentToken.substr(0, quotePosition);
                    extractSymbols(tempToken);
                    std::string stringLiteral = currentToken.substr(quotePosition);
                    stringLiteral += " ";

                    while(tokenStream){
                        currentToken.clear();
                        tokenStream >> currentToken;
                        stringLiteral += currentToken;
                        stringLiteral += " ";
                    }

                    stringLiteral = stringLiteral.substr(0, stringLiteral.length() - 2);
                    currentToken = stringLiteral;
                    extractSymbols(currentToken);
                }
            }
        }
    }

    cleanTokenVector();
}


void JackTokenizer::cleanTokenVector(){
    int a = 0;

    while(a < tokens.size()){
        if(tokens[a].length() == 0){
            tokens.erase(tokens.begin() + a);
            a--;
        }

        a++;
    }
}

std::string JackTokenizer::tokenType(std::string token){
    if(keywords.find(token) != keywords.end()){
        return "KEYWORD";
    }

    else if(symbols.find(token[0]) != symbols.end()){
        return "SYMBOL";
    }

    else if(isdigit(token[0])){
        return "INT_CONST";
    }

    else if(token[0] == '"' && token.back() == '"'){
        return "STRING_CONST";
    }

    else{
        return "IDENTIFIER";
    }
}

std::string JackTokenizer::keyWord(std::string token){
    std::unordered_set<std::string>::iterator tok = keywords.find(token);
    std::string key = *tok;
    std::string upperKey = "";

    for(int i = 0; i < key.length(); i++){
        upperKey += std::toupper(key[i]);
    }
    
    return upperKey;
}

char JackTokenizer::symbol(char token){
    std::unordered_set<char>::iterator tok = symbols.find(currentToken[0]);
    
    return *tok;
}

std::string JackTokenizer::identifier(std::string token){
    return token;
}

int JackTokenizer::intVal(){
    return std::stoi(currentToken);
}

void JackTokenizer::extractSymbols(std::string token){
    bool symbol_found = false;

    for(int i = 0; i < token.length(); i++){
        if(symbols.find(token[i]) != symbols.end()){
            symbol_found = true;
            break;
        }
    }

    if(symbol_found == false){
        tokens.push_back(token);
    }

    else{
        int j = 0;

        while(j < token.length()){
            if(symbols.find(token[j]) != symbols.end() && token.length() > 0){
                if(j > 0){
                    tokens.push_back(token.substr(0, j));
                    tokens.push_back(std::string(1, token[j]));
                    token.erase(0, j + 1);
                }

                else{
                    tokens.push_back(std::string(1, token[0]));
                    token = token.substr(1);
                }
                j=-1;
            }

            j++;
        }
    }

    if(token.length() > 0 && symbol_found == true){
        tokens.push_back(token);
    }
}

JackTokenizer::~JackTokenizer(){
    tokens.clear();
    if(inputFile.is_open()){
        inputFile.close();
    }
}