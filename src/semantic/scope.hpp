#pragma once

#include "../include/utils.hpp"
#include "../ast/astnode.hpp" 

#include "types.hpp"

#include <unordered_map>

struct Symbol {
    int lineDefinition;
    Type type;
};

struct Scope {
    Scope *parent = nullptr;
    std::unordered_map<std::string, Symbol> symbolTable;
};