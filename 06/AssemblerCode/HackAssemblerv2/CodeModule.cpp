#include "CodeModule.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

CodeModule::CodeModule(){
        aComp.insert(std::make_pair("0", "1110101010"));
        aComp.insert(std::make_pair("1", "1110111111"));
        aComp.insert(std::make_pair("-1", "1110111010"));
        aComp.insert(std::make_pair("D", "1110001100"));
        aComp.insert(std::make_pair("A", "1110110000"));
        aComp.insert(std::make_pair("!D", "1110001101"));
        aComp.insert(std::make_pair("!A", "1110110001"));
        aComp.insert(std::make_pair("-D", "1110001111"));
        aComp.insert(std::make_pair("-A", "1110110011"));
        aComp.insert(std::make_pair("D+1", "1110011111"));
        aComp.insert(std::make_pair("A+1", "1110110111"));
        aComp.insert(std::make_pair("D-1", "1110001110"));
        aComp.insert(std::make_pair("A-1", "1110110010"));
        aComp.insert(std::make_pair("D+A", "1110000010"));
        aComp.insert(std::make_pair("D-A", "1110010011"));
        aComp.insert(std::make_pair("A-D", "1110000111"));
        aComp.insert(std::make_pair("D&A", "1110000000"));
        aComp.insert(std::make_pair("D|A", "1110010101"));

        mComp.insert(std::make_pair("0", "1111101010"));
        mComp.insert(std::make_pair("1", "1111111111"));
        mComp.insert(std::make_pair("-1", "1111111010"));
        mComp.insert(std::make_pair("D", "1111001100"));
        mComp.insert(std::make_pair("M", "1111110000"));
        mComp.insert(std::make_pair("!D", "1111001101"));
        mComp.insert(std::make_pair("!M", "1111110001"));
        mComp.insert(std::make_pair("-D", "1111001111"));
        mComp.insert(std::make_pair("-M", "1111110011"));
        mComp.insert(std::make_pair("D+1", "1111011111"));
        mComp.insert(std::make_pair("M+1", "1111110111"));
        mComp.insert(std::make_pair("D-1", "1111001110"));
        mComp.insert(std::make_pair("M-1", "1111110010"));
        mComp.insert(std::make_pair("D+M", "1111000010"));
        mComp.insert(std::make_pair("D-M", "1111010011"));
        mComp.insert(std::make_pair("M-D", "1111000111"));
        mComp.insert(std::make_pair("D&M", "1111000000"));
        mComp.insert(std::make_pair("D|M", "1111010101"));

        destMap.insert(std::make_pair("null", "000"));
        destMap.insert(std::make_pair("M", "001"));
        destMap.insert(std::make_pair("D", "010"));
        destMap.insert(std::make_pair("MD", "011"));
        destMap.insert(std::make_pair("A", "100"));
        destMap.insert(std::make_pair("AM", "101"));
        destMap.insert(std::make_pair("AD", "110"));
        destMap.insert(std::make_pair("AMD", "111"));

        jumpMap.insert(std::make_pair("null", "000"));
        jumpMap.insert(std::make_pair("JGT", "001"));
        jumpMap.insert(std::make_pair("JEQ", "010"));
        jumpMap.insert(std::make_pair("JGE", "011"));
        jumpMap.insert(std::make_pair("JLT", "100"));
        jumpMap.insert(std::make_pair("JNE", "101"));
        jumpMap.insert(std::make_pair("JLE", "110"));
        jumpMap.insert(std::make_pair("JMP", "111"));
}

std::string CodeModule::dest(std::string destInput){
    return(destMap[destInput]);
}

std::string CodeModule::comp(std::string compInput){
    if(aComp.find(compInput) == aComp.end()){
        return mComp[compInput];
    }

    else{
        return aComp[compInput];
    }
}

std::string CodeModule::jump(std::string jumpInput){
    return(jumpMap[jumpInput]);
}