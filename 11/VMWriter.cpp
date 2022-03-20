#include "VMWriter.h"
#include <iostream>
#include <fstream>
#include <string>

VMWriter::VMWriter(std::string fileName){
    int dotPosition = fileName.find(".");
    outputFileName = fileName.substr(0, dotPosition);
    outputFileName += ".vm";
    outputFile.open(outputFileName);

    if(!outputFile.is_open()){
        std::cerr << "Could not open vm output file." << std::endl;
        return;
    }
}

void VMWriter::writePush(std::string segment, int index){
    outputFile << "push " << segment << " " << index << std::endl;
}

void VMWriter::writePop(std::string segment, int index){
    outputFile << "pop " << segment << " " << index << std::endl;
}

void VMWriter::writeArithmetic(std::string command){
    outputFile << command << std::endl;
}

void VMWriter::writeLabel(std::string label){
    outputFile << "label " << label << std::endl;
}

void VMWriter::writeGoto(std::string label){
    outputFile << "goto " << label << std::endl;
}

void VMWriter::writeIf(std::string label){
    outputFile << "if-goto " << label << std::endl;
}

void VMWriter::writeCall(std::string name, int nArgs){
    outputFile << "call " << name << " " << nArgs << std::endl;
}

void VMWriter::writeFunction(std::string name, int nVars){
    outputFile << "function " << name << " " << nVars << std::endl;
}

void VMWriter::writeReturn(){
    outputFile << "return" << std::endl;
}

void VMWriter::close(){
    if(outputFile.is_open()){
        outputFile.close();
    }
}