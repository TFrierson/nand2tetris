#pragma once

#include <unordered_map>
#include <string>

namespace symboltable{
    enum Kind {STATIC, FIELD, ARG, VAR, NONE};

    struct Identifier{
        std::string type;
        Kind kind;
        int index;
        std::string usage;
    };
}

class SymbolTable{
    public:
        SymbolTable();
        ~SymbolTable();
        void reset();
        void define(std::string name, std::string type, symboltable::Kind kind, std::string usage);
        int varCount(symboltable::Kind kind);
        std::string kindOf(std::string name);
        std::string typeOf(std::string name);
        int indexOf(std::string name);
        std::string usage(std::string name);
        void changeUsage(std::string name);
        bool isKey(std::string name);
        std::unordered_map<std::string, symboltable::Kind> stringtoKind;

    private:
        int field_index, static_index, var_index, arg_index;
        std::unordered_map<std::string, symboltable::Identifier> class_Table;
        std::unordered_map<std::string, symboltable::Identifier> subroutine_Table;
        
};