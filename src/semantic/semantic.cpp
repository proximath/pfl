#include "semantic.hpp"
#include "../parser/parser-utils.hpp"

SemanticAnalyzer::SemanticAnalyzer(AstNode *root){
    curNode = root;
    curScope = new Scope;
    analyze(curNode);
}

AstNode* SemanticAnalyzer::analyze(AstNode *root){
    
}
