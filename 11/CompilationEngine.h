#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

class CompilationEngine{
    public:
        CompilationEngine(std::string inputFile, JackTokenizer* jt, SymbolTable* st, VMWriter* vm);
        void compileClass();
        int compileClassVarDec(int index);
        int compileSubroutine(int index);
        int compileParameterList(int index);
        int compileSubroutineBody(int index);
        int compileVarDec(int index);
        int compileStatements(int index);
        int compileExpression(int index);
        int compileTerm(int index);
        int compileExpressionList(int index);
        int compileLet(int index);
        int compileIf(int index);
        int compileWhile(int index);
        int compileDo(int index);
        int compileReturn(int index);
        std::string stringToUpper(std::string input);
        std::string kindToSegment(std::string kind);
        
        JackTokenizer* jt_;
        SymbolTable* st_;
        VMWriter* vm_;
        std::string className;
        std::string subroutineName;
        std::string functionType;
        std::string functionName;
        int nArgs;
        int nVars;
        int whileLabelNo;
        int ifLabelNo;
};