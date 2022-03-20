#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

CompilationEngine::CompilationEngine(std::string inputFile, JackTokenizer* jt, SymbolTable* st){
    int dotPosition = inputFile.find('.');
    outputFileName = inputFile.substr(0, dotPosition);
    outputFileName += ".xml";
    outputFile.open(outputFileName);

    if(!outputFile.is_open()){
        std::cerr << "Error opening output file " << outputFileName << "." << std::endl;
    }

    jt_ = jt;
    st_ = st;
    compileClass();
}

CompilationEngine::~CompilationEngine(){
    if(outputFile.is_open()){
        outputFile.close();
    }
}


void CompilationEngine::compileClass(){
    outputFile << "<class>" << std::endl;
    int i = 0;

    assert(jt_->tokens[i] == "class");
    outputFile << "<keyword> class </keyword>" << std::endl;
    i++;
        
    if(jt_->tokenType(jt_->tokens[i]) == "IDENTIFIER"){
        outputFile << "<identifier> " << jt_->tokens[i] << " </identifier>" << std::endl;
        i++;
    }

    assert(jt_->tokens[i] == "{");
    outputFile << "<symbol> { </symbol>" << std::endl;
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
    outputFile << "<symbol> } </symbol>" << std::endl;
    outputFile << "</class>" << std::endl;
}


int CompilationEngine::compileClassVarDec(int index){
    outputFile << "<classVarDec>" << std::endl;

    //Static, Field
    outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" << std::endl;

    std::string type = "";
    symboltable::Kind kind = st_->stringtoKind[stringToUpper(jt_->tokens[index])];
    int lineIndex = 0;
    index++;

    while(jt_->tokens[index] != ";"){
        std::string name;
        //int, char
        if(jt_->tokenType(jt_->tokens[index]) == "KEYWORD"){
            outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" << std::endl;
            lineIndex = 1;
            type = jt_->tokens[index];
        }

        if(jt_->tokens[index] == ","){
            outputFile << "<symbol> , </symbol>" << std::endl;
        }

        if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
            outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" 
                << std::endl;

            if(lineIndex == 0){
                type = jt_->tokens[index];
            }

            else{
                name = jt_->tokens[index];
                st_->define(name, type, kind, "declared");
                outputFile << "<variable> " << name << "," << st_->typeOf(name)
                    << "," << st_->kindOf(name) << "," << st_->indexOf(name) << "," << st_->usage(name)
                    << " </variable>" << std::endl; 
            }
        }

    index++;
    }

    assert(jt_->tokens[index] == ";");
    outputFile << "<symbol> ; </symbol>" << std::endl;
    outputFile << "</classVarDec>" << std::endl;
    return(index);
}


int CompilationEngine::compileSubroutine(int index){
    st_->reset();
    outputFile << "<subroutineDec>" << std::endl;
    //eg CONSTRUCTOR | FUNCTION | METHOD
    outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" 
        << std::endl;
    
    index++;

    //Return type
    if(jt_->tokenType(jt_->tokens[index]) == "KEYWORD"){
        outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" 
            << std::endl;
        index++;
    }

    //eg subroutine's name
    else if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
        outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" << std::endl;
        index++;
    }

    if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
        outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" << std::endl;

        index++;
        
        if(jt_->tokens[index] == "("){
            outputFile << "<symbol> ( </symbol>" << std::endl;
            index = compileParameterList(index);

            if(jt_->tokens[index] == ")"){
            outputFile << "<symbol> ) </symbol>" << std::endl;
            index++;
            }
        }
        
        if(jt_->tokens[index] == "{"){
            index = compileSubroutineBody(index);

            if(jt_->tokens[index] == "}"){
                outputFile << "<symbol> } </symbol>" << std::endl;
            }

            outputFile << "</subroutineBody>" << std::endl;
        }        
    }

    outputFile << "</subroutineDec>" << std::endl;
    return(index);
}


int CompilationEngine::compileParameterList(int index){
    outputFile << "<parameterList>" << std::endl;
    symboltable::Kind kind = symboltable::ARG;
    std::string type = "";
    int lineIndex = 0;
    index++;

    while(jt_->tokens[index] != ")"){
        std::string name;
        if(jt_->tokenType(jt_->tokens[index]) == "KEYWORD"){
            outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" << std::endl;
            lineIndex = 1;
            type = jt_->tokens[index];
        }

        if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
            outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" << std::endl;

            if(lineIndex = 0){
                type = jt_->tokens[index];
                lineIndex = 1;
            }

            else{
                name = jt_->tokens[index];
                st_->define(name, type, kind, "used");
                outputFile << "<variable> " << name << "," << st_->typeOf(name)
                    << "," << st_->kindOf(name) << "," << st_->indexOf(name) << "," << st_->usage(name)
                    << " </variable>" << std::endl;
            }
        }

        if(jt_->tokenType(jt_->tokens[index]) == "SYMBOL"){
            outputFile << "<symbol> " << jt_->tokens[index] << " </symbol>" << std::endl;
            lineIndex = 0;
        }

        index++;
    }

    outputFile << "</parameterList>" << std::endl;
    return(index);
}


int CompilationEngine::compileSubroutineBody(int index){
    outputFile << "<subroutineBody>" << std::endl;
    outputFile << "<symbol> { </symbol>" << std::endl;
    index++;

    std::string token = jt_->tokens[index];
    while(token != "}"){
        if(token == "do" || token == "let" || token == "while" || token == "if" || token == "return"){
                index = compileStatements(index);
        }

        if(token == "var"){
            index = compileVarDec(index);
        }

        token = jt_->tokens[index];
    }

    return(index);
}

int CompilationEngine::compileVarDec(int index){
    outputFile << "<varDec>" << std::endl;
    std::string type = "";
    symboltable::Kind kind = symboltable::VAR;
    int lineIndex = 0;

    while(jt_->tokens[index] != ";"){
        std::string name;
        

        if(jt_->tokenType(jt_->tokens[index]) == "KEYWORD"){
            outputFile << "<keyword> " << jt_->tokens[index] << " </keyword>" << std::endl;

            if(jt_->tokens[index] != "var"){
                type = jt_->tokens[index];
                lineIndex = 1;
            }

            index++;
        }

        if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
            outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" << std::endl;

            //eg var Array a
            if(lineIndex == 0){
                type = jt_->tokens[index];
                lineIndex = 1;
            }

            else{
                name = jt_->tokens[index];

                st_->define(name, type, kind, "declared");
                outputFile << "<variable> " << name << "," << st_->typeOf(name)
                    << "," << st_->kindOf(name) << "," << st_->indexOf(name)  << "," << st_->usage(name)
                    << " </variable>" << std::endl;
            }

            index++;
        }

        if(jt_->tokens[index] == ","){
            outputFile << "<symbol> , </symbol>" << std::endl;
            index++;
        }
    }

    if(jt_->tokens[index] == ";"){
        outputFile << "<symbol> ; </symbol>" << std::endl;
        index++;
    }

    outputFile << "</varDec>" << std::endl;
    return(index);
}

int CompilationEngine::compileStatements(int index){
    outputFile << "<statements>" << std::endl;

    std::string token = jt_->tokens[index];

    if(token == "let" || token == "do" || token == "if" || token == "while"){
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

    outputFile << "</statements>" << std::endl;
    return(index);
}


int CompilationEngine::compileExpression(int index){
    outputFile << "<expression>" << std::endl;
    index = compileTerm(index);

    std::string token = jt_->tokens[index];

    while(token != ")" && token != "}" && token != "]" && token != ";" && token != ","){
        std::string tokenType = jt_->tokenType(token);

        if(tokenType == "SYMBOL"){
            if(token == "<"){
                outputFile << "<symbol> &lt; </symbol>" << std::endl;
            }

            else if(token == ">"){
                outputFile << "<symbol> &gt; </symbol>" << std::endl;
            }

            else if(token == "\""){
                outputFile << "<symbol> &quot; </symbol>" << std::endl;
            }

            else if(token == "&"){
                outputFile << "<symbol> &amp; </symbol>" << std::endl;
            }

            else{
                outputFile << "<symbol> " << token << " </symbol>" << std::endl;
            }

            index++;
            index = compileTerm(index);
            break;
        }

        else{
            index++;
            index = compileTerm(index);
            break;
        }

        token = jt_->tokens[index];
    }

    outputFile << "</expression>" << std::endl;
    return(index);
}


int CompilationEngine::compileTerm(int index){
    outputFile << "<term>" << std::endl;

    std::string token = jt_->tokens[index];
    std::string tokenType = jt_->tokenType(token);

    if(tokenType == "IDENTIFIER"){
        outputFile << "<identifier> " << token << " </identifier>" << std::endl;

        if(st_->isKey(token)){
            st_->changeUsage(token);
            outputFile << "<variable> " << token << "," << st_->typeOf(token)
                    << "," << st_->kindOf(token) << "," << st_->indexOf(token)  << "," 
                    << st_->usage(token) << " </variable>" << std::endl;
        }

        index++;

        if(jt_->tokens[index] == "("){
            outputFile << "<symbol> ( </symbol>" << std::endl;
            index++;
            index = compileExpression(index);

            if(jt_->tokens[index] == ")"){
                outputFile << "<symbol> ) </symbol>" << std::endl;
                index++;
            }
        }

        else if(jt_->tokens[index] == "."){
            outputFile << "<symbol> . </symbol>" << std::endl;
            index++;
            outputFile << "<identifier> " << jt_->tokens[index] << "</identifier>" << std::endl;

            if(st_->isKey(token)){
                st_->changeUsage(token);
                outputFile << "<variable> " << token << "," << st_->typeOf(token)
                        << "," << st_->kindOf(token) << "," << st_->indexOf(token)  << "," 
                        << st_->usage(token) << " </variable>" << std::endl;
            }

            index++;

            if(jt_->tokens[index] == "("){
                outputFile << "<symbol> ( </symbol>" << std::endl;
                index++;
                index = compileExpressionList(index);

                if(jt_->tokens[index] == ")"){
                    outputFile << "<symbol> ) </symbol>" << std::endl;
                    index++;
                }
            }
        }

        else if(jt_->tokens[index] == "["){
            outputFile << "<symbol> [ </symbol>" << std::endl;
            index++;

            index = compileExpression(index);
            if(jt_->tokens[index] == "]"){
                outputFile << "<symbol> ] </symbol>" << std::endl;
                index++;
            }
        }
    }

    if(tokenType == "INT_CONST"){
        outputFile << "<integerConstant> " << token << " </integerConstant>" << std::endl;
        index++;
    }

    if(tokenType == "STRING_CONST"){
        outputFile << "<stringConstant> " << token.substr(1, token.length() - 2) 
            << " </stringConstant>" << std::endl;
        index++;
    }

    if(tokenType == "KEYWORD"){
        outputFile << "<keyword> " << token << " </keyword>" << std::endl;
        index++;
    }

    if(tokenType == "SYMBOL"){
        if(token == "("){
            outputFile << "<symbol> ( </symbol>" << std::endl;
            index++;
            index = compileExpression(index);
            assert(jt_->tokens[index] == ")");
            outputFile << "<symbol> ) </symbol>" << std::endl;
            index++;
        }

        else{
            outputFile << "<symbol> " << token << " </symbol>" << std::endl;
            index++;
            index = compileTerm(index);
        }
    }

    outputFile << "</term>" << std::endl;
    return(index);
}


int CompilationEngine::compileExpressionList(int index){
    outputFile << "<expressionList>" << std::endl;

    std::string token = jt_->tokens[index];

    while(token != ")"){
        index = compileExpression(index);

        if(jt_->tokens[index] == ","){
            outputFile << "<symbol> , </symbol>" << std::endl;
            index++;
        }

        token = jt_->tokens[index];
    }

    outputFile << "</expressionList>" << std::endl;
    return(index);
}


int CompilationEngine::compileLet(int index){
    outputFile << "<letStatement>" << std::endl;
    outputFile << "<keyword> let </keyword>" << std::endl;
    index++;

    if(jt_->tokenType(jt_->tokens[index]) == "IDENTIFIER"){
        outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" << std::endl;
        index++;
    }

    std::string token = jt_->tokens[index];
    while(token != ";"){

        if(jt_->tokens[index] == "["){
            outputFile << "<symbol> [ </symbol>" << std::endl;
            index++;
            index = compileExpression(index);

            if(jt_->tokens[index] == "]"){
                outputFile << "<symbol> ] </symbol>" << std::endl;
                index++;
            }
        }

        if(jt_->tokens[index] == "="){
            outputFile << "<symbol> = </symbol>" << std::endl;
            index++;
            index = compileExpression(index);
        }

        token = jt_->tokens[index];
    }

    if(jt_->tokens[index] == ";"){
        outputFile << "<symbol> ; </symbol>" << std::endl;
        index++;
    }

    outputFile << "</letStatement>" << std::endl;
    return(index);
}


int CompilationEngine::compileDo(int index){
    outputFile << "<doStatement>" << std::endl;
    outputFile << "<keyword> do </keyword>" << std::endl;
    index++;

    //subroutine call
    std::string token = jt_->tokens[index];
    while(token != ";"){
        outputFile << "<identifier> " << jt_->tokens[index] << " </identifier>" << std::endl;
        index++;

        if(jt_->tokens[index] == "."){
            outputFile << "<symbol> . </symbol>" << std::endl;
            index++;
        }

        if(jt_->tokens[index] == "("){
            outputFile << "<symbol> ( </symbol>" << std::endl;
            index++;
            index = compileExpressionList(index);
            

            if(jt_->tokens[index] == ")"){
                outputFile << "<symbol> ) </symbol>" << std::endl;
                index++;
            }
        }

        token = jt_->tokens[index];
    }

    if(jt_->tokens[index] == ";"){
        outputFile << "<symbol> ; </symbol>" << std::endl;
    }

    outputFile << "</doStatement>" << std::endl;
    return(index);
}


int CompilationEngine::compileIf(int index){
    outputFile << "<ifStatement>" << std::endl;
    outputFile << "<keyword> if </keyword>" << std::endl;
    index++;

    if(jt_->tokens[index] == "("){
        outputFile << "<symbol> ( </symbol>" << std::endl;
        index++;
        index = compileExpression(index);

        if(jt_->tokens[index] == ")"){
            outputFile << "<symbol> ) </symbol>" << std::endl;
            index++;
        }
    }

    if(jt_->tokens[index] == "{"){
        outputFile << "<symbol> { </symbol>" << std::endl;
        index++;

        std::string token = jt_->tokens[index];

        while(token != "}"){
            index = compileStatements(index);
            token = jt_->tokens[index];
        }

        if(jt_->tokens[index] == "}"){
            outputFile << "<symbol> } </symbol>" << std::endl;
            index++;
        }
    }

    if(jt_->tokens[index] == "else"){
        outputFile << "<keyword> else </keyword>" << std::endl;
        index++;

        if(jt_->tokens[index] == "{"){
            outputFile << "<symbol> { </symbol>" << std::endl;
            index++;

            std::string token_ = jt_->tokens[index];

            while(token_ != "}"){
                index = compileStatements(index);
                token_ = jt_->tokens[index];
            }

            if(jt_->tokens[index] == "}"){
                outputFile << "<symbol> } </symbol>" << std::endl;
                index++;
            }
        }
    }

    outputFile << "</ifStatement>" << std::endl;
    return(index);
}

int CompilationEngine::compileWhile(int index){
    outputFile << "<whileStatement>" << std::endl;
    outputFile << "<keyword> while </keyword>" << std::endl;
    index++;

    if(jt_->tokens[index] != "("){
        index++;
    }

    assert(jt_->tokens[index] == "(");
    outputFile << "<symbol> ( </symbol>" << std::endl;
    index++;

    index = compileExpression(index);

    assert(jt_->tokens[index] == ")");
    outputFile << "<symbol> ) </symbol>" << std::endl;
    index++;

    assert(jt_->tokens[index] == "{");
    outputFile << "<symbol> { </symbol>" << std::endl;
    index++;
    std::string token = jt_->tokens[index];

    while(token != "}"){
        index = compileStatements(index);
        token = jt_->tokens[index];
    }

    assert(jt_->tokens[index] == "}");
    outputFile << "<symbol> } </symbol>" << std::endl;
    index++;

    outputFile << "</whileStatement>" << std::endl;
    return(index);
}


int CompilationEngine::compileReturn(int index){
    outputFile << "<returnStatement>" << std::endl;
    outputFile << "<keyword> return </keyword>" << std::endl;
    
    index++;

    std::string token = jt_->tokens[index];
    while(token != ";"){
        index = compileExpression(index);
        token = jt_->tokens[index];
    }

    if(jt_->tokens[index] == ";"){
        outputFile << "<symbol> ; </symbol>" << std::endl;
        index++;
    }

    outputFile << "</returnStatement>" << std::endl;

    return(index);
}


std::string CompilationEngine::stringToUpper(std::string input){
    std::string upperString = "";

    for(int i = 0; i < input.length(); i++){
        upperString += toupper(input[i]);
    }

    return upperString;
}