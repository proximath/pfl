#include "../include/utils.hpp"
#include "../ast/astnode.hpp"

#include "scope.hpp"

class SemanticAnalyzer {
private:
    AstNode *curNode;
    Scope *curScope;
public:
    SemanticAnalyzer(AstNode*);
    AstNode* analyze(AstNode*);
};