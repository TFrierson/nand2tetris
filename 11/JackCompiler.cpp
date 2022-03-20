#include <iostream>
#include <fstream>
#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include "VMWriter.h"

int main(int argc, char* argv[]){
    if(argc == 0){
        std::cout << "Usage: JackCompiler file1.jack file2.jack ..." << std::endl;
        return -1;
    }

    std::string fileName;
    for(int i = 1; i < argc; i++){
        
        fileName = argv[i];
        int fileExt = fileName.find(".jack");
        if(fileExt == std::string::npos){
            continue;
        }

        else{
            JackTokenizer jt(fileName);

            while(jt.hasMoreTokens()){
                jt.advance();
            }

            SymbolTable st;
            VMWriter vm(fileName);
            CompilationEngine ce(fileName, &jt, &st, &vm);
        }
    }

    return 0;
}