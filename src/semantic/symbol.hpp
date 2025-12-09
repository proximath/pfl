#pragma once

#include "../include/utils.hpp"
#include "../ast/astnode.hpp" 

#include "types.hpp"

#include <unordered_map>

enum class SymbolKind {
    primitive,
    structure,
    enumeration,
    variable,
    attribute,
    function
};

struct Symbol {
    std::string name;
    SymbolKind kind;
    Type *type;
};

static bool isType(const Symbol &sym){
    return sym.kind == SymbolKind::structure ||
        sym.kind == SymbolKind::enumeration ||
        sym.kind == SymbolKind::primitive;
}

struct Scope {
    Scope *parent = nullptr;
    std::unordered_map<std::string, Symbol> symbolTable;
};