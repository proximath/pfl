#pragma once

#include "../include/utils.hpp"
#include "../ast/astnode.hpp"

#include "symbol.hpp"

static std::vector<Type> types;

static constexpr const int NONE_INDEX = 0;
static constexpr const int INT_INDEX = 1;
static constexpr const int FLOAT_INDEX = 2;
static constexpr const int BOOL_INDEX = 3;
static constexpr const int STR_INDEX = 4;
static constexpr const int TYPE_INDEX = 5;

class SemanticAnalyzer {
private:
    ExprNode *curNode;
    Scope *curScope;
    Type& getPrimaryType(ExprNode*);
    Type& typeCheckExpr(ExprNode*);
    void precomputePrimary(ExprNode*);
    void emitError(const std::string&);
    bool isAlreadyInScope(const std::string&);
    Symbol* findSymbol(const std::string&, Scope*);
    Scope* pushNewScope();
    Scope* popScope();
    Type* promoteIfNeeded(Type*, Type*, ExprNode*, ExprNode*, ExprNode*);
    Type* analyzeAssignment(ExprNode*);
public:
    SemanticAnalyzer(ExprNode*);
    Type& analyze(ExprNode*);
};