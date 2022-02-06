#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

class CodeModule{
    public:
        CodeModule();

        std::string dest(std::string destInput);
        std::string comp(std::string compInput);
        std::string jump(std::string jumpInput);

    private:
        std::unordered_map<std::string, std::string> aComp;
        std::unordered_map<std::string, std::string> mComp;
        std::unordered_map<std::string, std::string> destMap;
        std::unordered_map<std::string, std::string> jumpMap;
};