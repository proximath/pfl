#include "semantic.hpp"

void SemanticAnalyzer::emitError(const std::string &msg){
	throw SemanticError(msg);
}

Scope* SemanticAnalyzer::pushNewScope(){
    curScope = new Scope{ curScope };
    return curScope;
}

Scope* SemanticAnalyzer::popScope(){
    if(curScope == nullptr){
        throw SystemError("There's no scope to pop", __FILE_NAME__, __LINE__);
    }
    Scope *temp = curScope;
    curScope = curScope->parent;
    delete temp;
    return curScope;
}

AstNode* insertImplicitCastNode(Type &fromType, Type &toType, AstNode *expr){
    AstNode  *returned;
    if(toType.index == FLOAT_INDEX){
        returned = createNode(NodeType::castToFloat);
        returned->valueType = &types[FLOAT_INDEX];
        returned->as<CastToFloat>().expr = expr;
    } else if(toType.index == INT_INDEX){
        returned = createNode(NodeType::castToInt);
        returned->valueType = &types[INT_INDEX];
        returned->as<CastToInt>().expr = expr;
    }
    return returned;
}

bool isImplicitCastable(Type &from, Type &to){
    if(from.index == INT_INDEX && to.index == FLOAT_INDEX){
        return true;
    }
    return false;
}

Type* SemanticAnalyzer::promoteIfNeeded(Type *leftType, Type *rightType, AstNode *left, AstNode *right, AstNode *parent){
    if(isImplicitCastable(*leftType, *rightType)){
            parent->as<BinaryOperation>().left = insertImplicitCastNode(
                *leftType, 
                *rightType, 
                parent->as<BinaryOperation>().left 
            );
            return rightType;
        } else if(isImplicitCastable(*rightType, *leftType)){
            parent->as<BinaryOperation>().right = insertImplicitCastNode(
                *rightType, 
                *leftType, 
                parent->as<BinaryOperation>().right 
            );
            return leftType;
        } else {
            return leftType;
        }
}