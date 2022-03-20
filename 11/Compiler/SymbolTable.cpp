#include "JackTokenizer.h"
#include "SymbolTable.h"

SymbolTable::SymbolTable(){
    field_index = static_index = var_index = arg_index = 0;
    class_Table.clear();
    subroutine_Table.clear();
    stringtoKind = {{"STATIC", symboltable::Kind::STATIC}, {"FIELD", symboltable::Kind::FIELD},
                {"VAR", symboltable::Kind::VAR}, {"ARG", symboltable::Kind::ARG},
                {"NONE", symboltable::Kind::NONE}};
}

void SymbolTable::reset(){
    subroutine_Table.clear();
    var_index = 0;
    arg_index = 0;
}

void SymbolTable::define(std::string name, std::string type, symboltable::Kind kind, std::string usage){
    if(kind == symboltable::STATIC){
        class_Table[name].type = type;
        class_Table[name].kind = symboltable::Kind::STATIC;
        class_Table[name].index = static_index;
        class_Table[name].usage = usage;
        static_index++;
    }

    else if(kind == symboltable::FIELD){
        class_Table[name].type = type;
        class_Table[name].kind = symboltable::Kind::FIELD;
        class_Table[name].index = field_index;
        class_Table[name].usage = usage;
        field_index++;
    }

    else if(kind == symboltable::VAR){
        subroutine_Table[name].type = type;
        subroutine_Table[name].kind = symboltable::Kind::VAR;
        subroutine_Table[name].index = var_index;
        subroutine_Table[name].usage = usage;
        var_index++;
    }

    else if(kind == symboltable::ARG){
        subroutine_Table[name].type = type;
        subroutine_Table[name].kind = symboltable::Kind::ARG;
        subroutine_Table[name].index = arg_index;
        subroutine_Table[name].usage = usage;
        arg_index++;
    }
}


int SymbolTable::varCount(symboltable::Kind kind){
    switch(kind){
        case symboltable::STATIC :
            return static_index;

        case symboltable::FIELD :
            return field_index;

        case symboltable::VAR :
            return var_index;

        case symboltable::ARG :
            return arg_index;
    }
}


std::string SymbolTable::kindOf(std::string name){
    static const char* EnumTable[] {"STATIC", "FIELD", "ARG", "VAR"};
    if(class_Table.find(name) != class_Table.end()){
        return (EnumTable[class_Table[name].kind]);
    }

    else if(subroutine_Table.find(name) != subroutine_Table.end()){
        return (EnumTable[subroutine_Table[name].kind]);
    }

    else{
        return "Error";
    }
}


std::string SymbolTable::typeOf(std::string name){
    if(class_Table.find(name) != class_Table.end()){
        return(class_Table[name].type);
    }

    else if(subroutine_Table.find(name) != subroutine_Table.end()){
        return(subroutine_Table[name].type);
    }

    else{
        return "Error";
    }
}


int SymbolTable::indexOf(std::string name){
    if(class_Table.find(name) != class_Table.end()){
        return(class_Table[name].index);
    }

    else if(subroutine_Table.find(name) != subroutine_Table.end()){
        return(subroutine_Table[name].index);
    }

    else{
        return(-1);
    }
}


bool SymbolTable::isKey(std::string name){
    bool isKey = false;

    if(class_Table.find(name) != class_Table.end() || subroutine_Table.find(name) != subroutine_Table.end()){
        return true;
    }

    return(isKey);
}


std::string SymbolTable::usage(std::string name){
    if(class_Table.find(name) != class_Table.end()){
        return(class_Table[name].usage);
    }

    else if(subroutine_Table.find(name) != subroutine_Table.end()){
        return(subroutine_Table[name].usage);
    }
}


void SymbolTable::changeUsage(std::string name){
    if(class_Table.find(name) != class_Table.end()){
        class_Table[name].usage = "used";
    }

    else if(subroutine_Table.find(name) != subroutine_Table.end()){
        subroutine_Table[name].usage = "used";
    }
}


SymbolTable::~SymbolTable(){
    class_Table.clear();
    subroutine_Table.clear();
}