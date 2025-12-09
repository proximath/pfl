#include "../include/utils.hpp"
#include "../ast/astnode.hpp"

#include "scope.hpp"

static std::vector<Type> types;
static std::unordered_map<std::string, int> typeIndex;

class SemanticAnalyzer {
private:
    AstNode *curNode;
    Scope *curScope;
    Type& getPrimaryType(AstNode*);
    Type& typeCheckExpr(AstNode*);
    void precomputePrimary(AstNode*);
    void emitError(const std::string&);
public:
    SemanticAnalyzer(AstNode*);
    Type& analyze(AstNode*);
};