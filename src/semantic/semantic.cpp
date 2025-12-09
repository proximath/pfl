#include "semantic.hpp"
#include "../parser/parser-utils.hpp"

constexpr const int INT_INDEX = 0;
constexpr const int FLOAT_INDEX = 1;
constexpr const int STR_INDEX = 2;

SemanticAnalyzer::SemanticAnalyzer(AstNode *root){
    curNode = root;
    curScope = new Scope;
    types.push_back({ Type{ "int", true, INT_INDEX } });
    types.push_back({ Type{ "float", true, FLOAT_INDEX } });
    types.push_back({ Type{ "str", true, STR_INDEX } });
    typeIndex.insert({ 
        { "int", INT_INDEX }, 
        { "float", FLOAT_INDEX }, 
        { "str", STR_INDEX } 
    });
    analyze(curNode);
}

Type& SemanticAnalyzer::analyze(AstNode *root){
    return typeCheckExpr(root);
}

void SemanticAnalyzer::emitError(const std::string &msg){
	throw SemanticError(msg);
}

Type& SemanticAnalyzer::getPrimaryType(AstNode *primary){
    switch (primary->type) {
    case NodeType::intLiteral:
        return types[typeIndex["int"]];
    break;
    case NodeType::floatLiteral:
        return types[typeIndex["float"]];
    break;
    default:
        throw SystemError("analyzePrimary More primary types to come");
    }
}

void SemanticAnalyzer::precomputePrimary(AstNode *primary){
    switch (primary->type) {
    case NodeType::intLiteral:
        primary->as<IntLiteral>().precomputed = stoi(
            primary->as<IntLiteral>().value
        );
    break;
    case NodeType::floatLiteral:
        primary->as<FloatLiteral>().precomputed = stod(
            primary->as<FloatLiteral>().value
        );
    break;
    default:
        throw SystemError("analyzePrimary More primary types to come");
    }
}

AstNode* insertImplicitCastNode(Type &fromType, Type &toType, AstNode *expr){
    AstNode  *returned;
    if(toType.index == FLOAT_INDEX){
        returned = createNode(NodeType::castToFloat);
        returned->as<CastToFloat>().expr = expr;
    } else if(toType.index == INT_INDEX){
        returned = createNode(NodeType::castToInt);
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

bool isBinaryArithOperation(NodeType type){
    return type == NodeType::addition ||
        type == NodeType::subtraction ||
        type == NodeType::multiplication ||
        type == NodeType::division ||
        type == NodeType::exponentiation;
}

Type& SemanticAnalyzer::typeCheckExpr(AstNode *node){
    if(isPrimary(node->type)){
        precomputePrimary(node);
        return getPrimaryType(node);
    }
    if(isBinaryArithOperation(node->type)){
        Type &left = typeCheckExpr(node->as<BinaryOperation>().left);
        Type &right = typeCheckExpr(node->as<BinaryOperation>().right);
        if(isImplicitCastable(left, right)){
            node->as<BinaryOperation>().left = insertImplicitCastNode(
                left, 
                right, 
                node->as<BinaryOperation>().left 
            );
            node->as<BinaryOperation>().type = &right;
        } else if(isImplicitCastable(right, left)){
            node->as<BinaryOperation>().right = insertImplicitCastNode(
                right, 
                left, 
                node->as<BinaryOperation>().right 
            );
            node->as<BinaryOperation>().type = &left;
        } else {
            node->as<BinaryOperation>().type = &left;
        }
        return *(node->as<BinaryOperation>().type); 
    }
    switch(node->type){
    case NodeType::block:
        int resultInd;
        for(AstNode *child : node->as<Block>().expressions){
            resultInd = typeCheckExpr(child).index;
        }
        return types[resultInd];
    break;
    default:
        throw SystemError("SemanticAnalyzer::typeCheckExpr node type unimplemented", __FILE_NAME__, __LINE__);
    }
}