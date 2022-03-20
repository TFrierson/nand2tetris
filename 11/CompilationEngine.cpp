#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>

CompilationEngine::CompilationEngine(std::string inputFile, JackTokenizer* jt, 
                                        SymbolTable* st, VMWriter* vm){

    jt_ = jt;
    st_ = st;
    vm_ = vm;
    whileLabelNo = 0;
    ifLabelNo = 0;
    compileClass();
}


void CompilationEngine::compileClass(){
    int i = 0;

    assert(jt_->tokens[i] == "class");
    i++;
        
    if(jt_->tokenType(jt_->tokens[i]) == "IDENTIFIER"){
        className = jt_->tokens[i];
        i++;
    }

    assert(jt_->tokens[i] == "{");
    i++;

    //The class body will contain class variables and/or class functions
    std::string token = jt_->tokens[i];

    while(i < jt_->tokens.size() && jt_->tokenType(token) == "KEYWORD"){
        std::string key = jt_->keyWord(jt_->tokens[i]);

        if(key == "STATIC" || key == "FIELD"){
            i = compileClassVarDec(i);
        }
        
        if(key == "CONSTRUCTOR" || key == "FUNCTION" || key == "METHOD" || key == "VOID"){
            i = compileSubroutine(i);   
        }

        i++;
        token = jt_->tokens[i];
    }

    assert(jt_->tokens[i] == "}");
}


int CompilationEngine::compileClassVarDec(int index){
    //Static, Field
    //outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" << std::endl;

    std::string name;
    std::string type;
    symboltable::Kind kind = st_->stringtoKind[stringToUpper(jt_->tokens[index])];
    int lineIndex = 0;
    index++;

    while(jt_->tokens[index] != ";"){
        //int, char
        if(jt_->tokenType(jt_->tokens[index]) == "KEYWORD"){
            lineIndex = 1;
            type = jt_->tokens[index];
        }

        if(jt_->tokens[index] == ","){
        }

        if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
            //outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" 
                //<< std::endl;

            if(lineIndex == 0){
                type = jt_->tokens[index];
                lineIndex = 1;
            }

            else{
                name = jt_->tokens[index];
                st_->define(name, type, kind, "declared");
            }
        }

        index++;
    }

    assert(jt_->tokens[index] == ";");
    return(index);
}


int CompilationEngine::compileSubroutine(int index){
    st_->reset();
    functionName = "";
    //eg CONSTRUCTOR | FUNCTION | METHOD
    /*outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" 
        << std::endl;*/
    functionType = jt_->tokens[index];

    nVars = 0;

    if(functionType == "method"){
        st_->define("this", className, symboltable::ARG, "defined");
        functionName = className;
    }
    
    index++;


    if(jt_->tokenType(jt_->tokens[index]) == "KEYWORD"){
        //eg void
        index++;
    }


    else if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){

        if(functionType == "constructor"){
            functionName = jt_->tokens[index];
        }

        index++;
    }

    if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
        subroutineName = jt_->tokens[index];

        index++;

        if(jt_->tokens[index] == "("){
            index = compileParameterList(index);

            if(jt_->tokens[index] == ")"){
            index++;
            }

            if(functionName.length() > 0){
                functionName = functionName + "." + subroutineName;
            }

            else{
                functionName = className + "." + subroutineName;
            }
        }
        
        if(jt_->tokens[index] == "{"){
            index = compileSubroutineBody(index);

            if(jt_->tokens[index] == "}"){
                //outputFile << "<symbol> } </symbol>" << std::endl;
            }

            //outputFile << "</subroutineBody>" << std::endl;
        }        
    }

    return(index);
}


int CompilationEngine::compileParameterList(int index){
    //outputFile << "<parameterList>" << std::endl;
    symboltable::Kind kind = symboltable::ARG;
    std::string type = "";
    int lineIndex = 0;
    index++;

    while(jt_->tokens[index] != ")"){
        std::string name;
        if(jt_->tokenType(jt_->tokens[index]) == "KEYWORD"){
            //outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" << std::endl;
            lineIndex = 1;
            type = jt_->tokens[index];
        }

        if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
            //outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" << std::endl;

            if(lineIndex = 0){
                type = jt_->tokens[index];
                lineIndex = 1;
            }

            else{
                name = jt_->tokens[index];
                st_->define(name, type, kind, "declared");
            }
        }

        if(jt_->tokenType(jt_->tokens[index]) == "SYMBOL"){
            //outputFile << "<symbol> " << jt_->tokens[index] << " </symbol>" << std::endl;
            lineIndex = 0;
        }

        index++;
    }

    //outputFile << "</parameterList>" << std::endl;
    return(index);
}


int CompilationEngine::compileSubroutineBody(int index){
    //outputFile << "<symbol> { </symbol>" << std::endl;
    index++;

    int tempIndex = index;
    std::string token = jt_->tokens[tempIndex];

    while(token == "var"){
        tempIndex = compileVarDec(tempIndex);
        token = jt_->tokens[tempIndex];
    }

    int varCount = st_->varCount(symboltable::VAR);

    vm_->writeFunction(functionName, varCount);

    if(functionType == "method"){
        vm_->writePush("argument", 0);
        vm_->writePop("pointer", 0);
    }

    if(functionType == "constructor"){
        int nFields = st_->varCount(symboltable::FIELD);
        vm_->writePush("constant", nFields);
        vm_->writeCall("Memory.alloc", 1);
        vm_->writePop("pointer", 0);
    }

    token = jt_->tokens[index];
    while(token != "}"){
        if(token == "do" || token == "let" || token == "while" || token == "if" || token == "return"){
                index = compileStatements(index);
        }

        else{
            index++;
        }

        token = jt_->tokens[index];
    }

    return(index);
}


int CompilationEngine::compileVarDec(int index){
    //outputFile << "<varDec>" << std::endl;
    std::string type = "";
    symboltable::Kind kind = symboltable::VAR;
    std::string name;
    int lineIndex = 0;
    std::string varToken = jt_->tokens[index];

    while(varToken != ";"){
        //std::string name;

        if(jt_->tokenType(varToken) == "KEYWORD"){

            //ie boolean, char
            if(varToken != "var"){
                type = varToken;
                lineIndex = 1;
            }

            index++;
        }

        if(jt_->tokenType(varToken) == "IDENTIFIER"){
            //eg var Array a
            if(lineIndex == 0){
                type = varToken;
                lineIndex = 1;
            }

            else{
                name = varToken;
                st_->define(name, type, kind, "declared");
                nVars++;
            }

            index++;
        }

        if(varToken == ","){
            //outputFile << "<symbol> , </symbol>" << std::endl;
            lineIndex = 1;
            index++;
        }

        varToken = jt_->tokens[index];
    }

    
    if(jt_->tokens[index] == ";"){
        //outputFile << "<symbol> ; </symbol>" << std::endl;
        index++;
    }

    //outputFile << "</varDec>" << std::endl;
    return(index);
}

int CompilationEngine::compileStatements(int index){
    //outputFile << "<statements>" << std::endl;

    std::string token = jt_->tokens[index];

    if(token == "let" || token == "do" || token == "if" || token == "while" || token == "return"){
        while(token != ";" && token != "}"){
            if(token == "let"){
                index = compileLet(index);
            }

            if(token == "do"){
                index = compileDo(index);
                index++;
            }

            if(token == "if"){
                index = compileIf(index);
            }

            if(token == "while"){
                index = compileWhile(index);
            }

            if(token == "return"){
                index = compileReturn(index);
            }

            token = jt_->tokens[index];
            
        }
    }

    //outputFile << "</statements>" << std::endl;
    return(index);
}


int CompilationEngine::compileExpression(int index){
    //outputFile << "<expression>" << std::endl;
    index = compileTerm(index);
    std::string op;

    std::string token = jt_->tokens[index];

    while(token != ")" && token != "}" && token != "]" && token != ";" && token != ","){
        std::string tokenType = jt_->tokenType(token);

        if(tokenType == "SYMBOL"){
            if(token == "<"){
                op = "lt";
            }

            else if(token == ">"){
                op = "gt";
            }

            else if(token == "&"){
                op = "and";
            }

            else if(token == "+"){
                op = "add";
            }

            else if(token == "-"){
                op = "sub";
            }

            else if(token == "|"){
                op = "or";
            }

            else if(token == "~"){
                op = "not";
            }

            else if(token == "*"){
                op = "multiply";
            }

            else if(token == "/"){
                op = "divide";
            }

            else if(token == "="){
                op = "eq";
            }

            index++;
            index = compileTerm(index);

            if(op == "multiply" || op == "divide"){
                std::string mathCall = "Math." + op;
                vm_->writeCall(mathCall, 2);
            }

            else{
                vm_->writeArithmetic(op);
            }
            break;
        }

        else{
            index++;
            index = compileTerm(index);
            break;
        }

        token = jt_->tokens[index];
    }

    //outputFile << "</expression>" << std::endl;
    return(index);
}


int CompilationEngine::compileTerm(int index){
    //outputFile << "<term>" << std::endl;
    std::string token = jt_->tokens[index];
    std::cout << "token is: " << token << std::endl;
    std::string tokenType = jt_->tokenType(token);

    if(tokenType == "IDENTIFIER"){
        if(st_->isKey(token)){
            std::string kind = st_->kindOf(token);
            std::string type = st_->typeOf(token);

            //If it is a class type (object), then we will call the class' method on it
            if(jt_->keywords.find(type) == jt_->keywords.end()){
                if(st_->typeOf(token) == "Array"){
                    if(jt_->tokens[index + 1] == "["){
                        index += 2;
                        index = compileExpression(index);
                        vm_->writePush(kindToSegment(st_->kindOf(token)), st_->indexOf(token));
                        vm_->writeArithmetic("add");
                        
                        if(jt_->tokens[index] == "]"){
                            index++;
                        }
                        vm_->writePop("pointer", 1);
                        vm_->writePush("that", 0);
                        return(index);
                    }

                    else{
                        vm_->writePush(kindToSegment(st_->kindOf(token)), st_->indexOf(token));
                    }
                }

                else{
                    functionName = st_->typeOf(token);
                    vm_->writePush(kindToSegment(kind), st_->indexOf(token));
                }
            }

            else{
                vm_->writePush(kindToSegment(kind), st_->indexOf(token));
            }
        }

        else{
            functionName = token;
        }

        index++;

        //subroutine call
        if(jt_->tokens[index] == "("){
            index++;
            vm_->writePush("pointer", 0);
            index = compileExpression(index);

            if(jt_->tokens[index] == ")"){
                index++;
            }
        }

        else if(jt_->tokens[index] == "."){
            index++;
            //outputFile << "<identifier> " << jt_->tokens[index] << "</identifier>" << std::endl;

            if(st_->isKey(token)){
                st_->changeUsage(token);
            }

            else{
                functionName += ".";
                functionName += jt_->tokens[index];
            }

            index++;

            if(jt_->tokens[index] == "("){
                index++;
                index = compileExpressionList(index);

                if(jt_->tokens[index] == ")"){
                    index++;
                }
            }

            vm_->writeCall(functionName, nArgs);
        }

        else if(jt_->tokens[index] == "["){
            index++;
            index = compileExpression(index);
            //vm_->writePush(kindToSegment(st_->kindOf(token)), st_->indexOf(token));
            vm_->writeArithmetic("add");
            
            if(jt_->tokens[index] == "]"){
                index++;
            }
            vm_->writePop("pointer", 1);
            vm_->writePush("that", 0);
        }
    }

    if(tokenType == "INT_CONST"){
        vm_->writePush("constant", stoi(token));
        index++;
    }

    if(tokenType == "KEYWORD"){
        //outputFile << "<keyword> " << token << " </keyword>" << std::endl;
        if(token == "true"){
            vm_->writePush("constant", 0);
            vm_->writeArithmetic("not");
        }

        else if(token == "false" || token == "null"){
            vm_->writePush("constant", 0);
        }

        else if(token == "this"){
            vm_->writePush("pointer", 0);
        }

        index++;
    }

    if(token[0] == '"'){
        std::string strConst = token.substr(1, token.length() - 2);
        int strLength = strConst.length();
        vm_->writePush("constant", strLength);
        vm_->writeCall("String.new", 1);

        for(int i = 0; i < strConst.length(); i++){
            vm_->writePush("constant", (int)strConst[i]);
            vm_->writeCall("String.appendChar", 2);
        }

        index++;
    }

    if(tokenType == "SYMBOL" && (token == "(" || token == "~" || token == "-")){
        if(token == "("){
            //outputFile << "<symbol> ( </symbol>" << std::endl;
            index++;
            index = compileExpression(index);
            assert(jt_->tokens[index] == ")");
            //outputFile << "<symbol> ) </symbol>" << std::endl;
            index++;
        }

        else{
            //unary op term
            std::string unOp;
            if(token == "~"){
                unOp = "not";
            }

            else if(token == "-"){
                unOp = "neg";
            }

            index++;
            index = compileTerm(index);
            vm_->writeArithmetic(unOp);
        }
    }

    //outputFile << "</term>" << std::endl;
    return(index);
}


int CompilationEngine::compileExpressionList(int index){
    //outputFile << "<expressionList>" << std::endl;
    nArgs = 0;

    std::string token = jt_->tokens[index];
    if(token == ")"){
        return(index);
    }

    while(token != ")" && token != ";"){
        index = compileExpression(index);
        nArgs++;
        if(jt_->tokens[index] == ","){
            //outputFile << "<symbol> , </symbol>" << std::endl;
            index++;
        }
            token = jt_->tokens[index];
    }
    
    return(index);
}


int CompilationEngine::compileLet(int index){
    bool isArray = false;
    index++;

    std::string lhSide;
    if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
        lhSide = jt_->tokens[index];
        index++;
    }

    if(jt_->tokens[index] == "["){
        isArray = true;
        index++;
        index = compileExpression(index);
        vm_->writePush(kindToSegment(st_->kindOf(lhSide)), st_->indexOf(lhSide));
        vm_->writeArithmetic("add");

        if(jt_->tokens[index] == "]"){
            index++;
        }
    }

    std::string token = jt_->tokens[index];
    while(token != ";"){

        if(jt_->tokens[index] == "["){
            //outputFile << "<symbol> [ </symbol>" << std::endl;
            isArray = true;
            index++;
            
            vm_->writePush(kindToSegment(st_->kindOf(jt_->tokens[index])), st_->indexOf(jt_->tokens[index]));
            index = compileExpression(index);
            vm_->writeArithmetic("add");

            if(jt_->tokens[index] == "]"){
                //outputFile << "<symbol> ] </symbol>" << std::endl;
                index++;
            }
        }

        if(jt_->tokens[index] == "="){
            index++;
            //right-hand side
            index = compileExpression(index);

            if(isArray){
                vm_->writePop("temp", 0);
            }
        }

        token = jt_->tokens[index];
    }

    if(jt_->tokens[index] == ";"){
        if(!isArray){
            std::string kind = st_->kindOf(lhSide);
            vm_->writePop(kindToSegment(kind), st_->indexOf(lhSide));
            index++;
        }

        else{
            vm_->writePop("pointer", 1);
            vm_->writePush("temp", 0);
            vm_->writePop("that", 0);
            index++;
        }
    }

    return(index);
}


int CompilationEngine::compileDo(int index){
    //outputFile << "<keyword> do </keyword>" << std::endl;
    index++;
    std::string doFunctionName = "";
    int lineIndex = 0;
    int outputArgs = 1;

    //subroutine call
    std::string token = jt_->tokens[index];
    while(token != ";"){
        //If the identifier is a key, then it is an object
        if(st_->isKey(token) && lineIndex == 0){
            std::string kind = st_->kindOf(token);
            vm_->writePush(kindToSegment(kind), st_->indexOf(token));
            doFunctionName = st_->typeOf(token);
            nArgs = 1;
            lineIndex = 1;
        }

        else{
            if(lineIndex == 0){
                int tempIndex = index;
                if(jt_->tokens[tempIndex + 1] != "."){
                    doFunctionName += className;
                    doFunctionName += ".";
                    doFunctionName += jt_->tokens[index];
                    vm_->writePush("pointer", 0);
                }

                else{
                    doFunctionName += jt_->tokens[index];
                    lineIndex = 1;
                    outputArgs = 0;
                }
            }

            else{
                doFunctionName += jt_->tokens[index];
            }
        }

        index++;

        if(jt_->tokens[index] == "."){
            doFunctionName += ".";
            lineIndex = 1;
            index++;
        }

        if(jt_->tokens[index] == "("){
            //outputFile << "<symbol> ( </symbol>" << std::endl;

            int tempToken = index + 1;
            if(jt_->tokens[tempToken] == ")"){
                nArgs = 0;
            }

            else{
                index++;
                index = compileExpressionList(index);
            }

            if(jt_->tokens[index] != ")"){
                index++;
            }

            if(jt_->tokens[index] == ")"){
                //outputFile << "<symbol> ) </symbol>" << std::endl;
                index++;
            }
        }

        token = jt_->tokens[index];
        //std::cout << "Token in do is now: " << token << std::endl;
    }

    if(nArgs == 0 && outputArgs == 1){
        nArgs = 1;
    }

    if(jt_->tokens[index] == ";"){
        vm_->writeCall(doFunctionName, nArgs);
        vm_->writePop("temp", 0);
    }

    return(index);
}

//Fix if and while compilation
int CompilationEngine::compileIf(int index){
    std::string ifLabel0 = "IF_FALSE" + std::to_string(ifLabelNo);
    std::string ifLabel1 = "CONTINUE" + std::to_string(ifLabelNo);
    ifLabelNo++;
    index++;

    if(jt_->tokens[index] == "("){
        //outputFile << "<symbol> ( </symbol>" << std::endl;
        index++;
        index = compileExpression(index);
        vm_->writeArithmetic("not");
        vm_->writeIf(ifLabel0);

        if(jt_->tokens[index] == ")"){
            
            index++;
        }
    }

    if(jt_->tokens[index] == "{"){
        //outputFile << "<symbol> { </symbol>" << std::endl;
        index++;

        std::string token = jt_->tokens[index];

        while(token != "}"){
            index = compileStatements(index);
            token = jt_->tokens[index];
        }

        vm_->writeGoto(ifLabel1);
        if(jt_->tokens[index] == "}"){
            
            index++;
        }
    }

    vm_->writeLabel(ifLabel0);
    if(jt_->tokens[index] == "else"){
        //outputFile << "<keyword> else </keyword>" << std::endl;
        index++;

        if(jt_->tokens[index] == "{"){
            index++;

            std::string token_ = jt_->tokens[index];

            while(token_ != "}"){
                index = compileStatements(index);
                token_ = jt_->tokens[index];
            }

            if(jt_->tokens[index] == "}"){
                index++;
            }
        }
    }

    vm_->writeLabel(ifLabel1);
    return(index);
}

int CompilationEngine::compileWhile(int index){
    std::string label0 = "WHILE_TRUE" + std::to_string(whileLabelNo);
    std::string label1 = "WHILE_FALSE" + std::to_string(whileLabelNo);
    whileLabelNo++;

    index++;

    if(jt_->tokens[index] != "("){
        index++;
    }

    assert(jt_->tokens[index] == "(");
    //outputFile << "<symbol> ( </symbol>" << std::endl;
    index++;
    vm_->writeLabel(label0);
    index = compileExpression(index);
    vm_->writeArithmetic("not");
    vm_->writeIf(label1);
    assert(jt_->tokens[index] == ")");
    //outputFile << "<symbol> ) </symbol>" << std::endl;

    index++;

    assert(jt_->tokens[index] == "{");
    //outputFile << "<symbol> { </symbol>" << std::endl;
    index++;
    std::string token = jt_->tokens[index];

    while(token != "}"){
        index = compileStatements(index);
        token = jt_->tokens[index];
    }

    assert(jt_->tokens[index] == "}");
    vm_->writeGoto(label0);
    //outputFile << "<symbol> } </symbol>" << std::endl;
    index++;

    vm_->writeLabel(label1);
    return(index);
}


int CompilationEngine::compileReturn(int index){
    //outputFile << "<keyword> return </keyword>" << std::endl;
    
    index++;

    std::string token = jt_->tokens[index];
    if(token == ";"){
        vm_->writePush("constant", 0);
    }

    else{
        while(token != ";"){
            index = compileExpression(index);
            token = jt_->tokens[index];
        }
    }

    if(jt_->tokens[index] == ";"){
        vm_->writeReturn();
        index++;
    }

    return(index);
}


std::string CompilationEngine::stringToUpper(std::string input){
    std::string upperString = "";

    for(int i = 0; i < input.length(); i++){
        upperString += toupper(input[i]);
    }

    return upperString;
}


std::string CompilationEngine::kindToSegment(std::string kind){
    if(kind == "ARG"){
        return "argument";
    }

    else if(kind == "VAR"){
        return "local";
    }

    else if(kind == "STATIC"){
        return "static";
    }

    else if(kind == "FIELD"){
        return "this";
    }

    else{
        return "none";
    }
}