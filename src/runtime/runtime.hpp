#pragma once

#include "../include/utils.hpp"
#include "../ast/astnode.hpp"
#include "type.hpp"

enum class RuntimeMode {
    repl,
    script
};

enum class SymbolType {
    type,
    variable
};

struct SymbolLookup {
    int stackDepth;
    SymbolType type;

    bool operator<(SymbolLookup ot){
        return stackDepth < ot.stackDepth;
    }
};

class Runtime {
private:
    RuntimeMode mode;
    std::unordered_map<std::string_view, Type> typeLookup;
public:
    Runtime getMode();
    std::string execute(AstNode*);
};