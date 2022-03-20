#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>

class JackTokenizer{
    public:
        std::ifstream inputFile;
        std::string currentLine;
        std::string currentToken;
        std::unordered_set<std::string> keywords;
        std::unordered_set<char> symbols;
        std::vector<std::string> tokens;
        std::vector<std::string> breakUpTokens;

        JackTokenizer(std::string fileInput);
        ~JackTokenizer();
        bool hasMoreTokens();
        void advance();
        std::string tokenType(std::string token);
        std::string keyWord(std::string token);
        char symbol(char token);
        std::string identifier(std::string token);
        int intVal();
        std::string stringVal();
        void cleanTokenVector();

    private:
        void extractSymbols(std::string token);
};