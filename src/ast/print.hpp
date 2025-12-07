#include "astnode.hpp"
#include "operator.hpp"

#include <iostream>

static void printAst(AstNode *node, int level = 0){
    for(int i = 0; i < 4 * level; i++){
        std::cout << " ";
    }
    if(node == nullptr){
        std::cout << "[NULL]" << std::endl;
        return;
    }
    std::cout << getNodeTypeName(node->type);
    if(isOperator(node->type)){
        std::cout << std::endl;
        if(isBinaryOperator(node->type)){
            printAst(node->as<BinaryOperation>().left, level + 1);
            printAst(node->as<BinaryOperation>().right, level + 1);
        } else {
            printAst(node->as<UnaryOperation>().expr, level + 1);
        }
        return;
    } 
    switch(node->type){
    case NodeType::identifier:
        std::cout << " | " << node->as<Identifier>().name << std::endl; 
    break;
    case NodeType::typedIdentifier:
        std::cout << " | ";
        std::cout << node->as<TypedIdentifier>().name << " | "; 
        std::cout << node->as<TypedIdentifier>().type << std::endl; 
    break;
    case NodeType::intLiteral:
        std::cout << " | " << node->as<IntLiteral>().value << std::endl; 
    break;
    case NodeType::floatLiteral:
        std::cout << " | " << node->as<FloatLiteral>().value << std::endl;
    break;
    case NodeType::stringLiteral:
        std::cout << " | " << node->as<StringLiteral>().value << std::endl;
    break;
    case NodeType::formatString:
        std::cout << std::endl;
        for(AstNode *child : node->as<FormatString>().children){
            printAst(child, level + 1);
        }
    break;
    case NodeType::stringTemplate:
        std::cout << std::endl;
        printAst(node->as<StringTemplate>().value, level + 1);
        printAst(node->as<StringTemplate>().format, level + 1);
    break;
    case NodeType::fnParamList:
        std::cout << std::endl;
        for(AstNode *param : node->as<FnParamList>().params){
            printAst(param, level + 1);
        }
    break;
    case NodeType::function:
        std::cout << std::endl;
        printAst(node->as<Function>().name, level + 1);
        printAst(node->as<Function>().paramList, level + 1);
        printAst(node->as<Function>().block, level + 1);
    break;
    case NodeType::block:
         std::cout << std::endl;
        for(AstNode *expr : node->as<Block>().expressions){
            printAst(expr, level + 1);
        }   
    break;
    case NodeType::ifExpr:
        std::cout << std::endl;
        printAst(node->as<IfExpr>().condition, level + 1);
        printAst(node->as<IfExpr>().ifBlock, level + 1);
        for(int i = 0; i < node->as<IfExpr>().elifBlock.size(); i++){
            printAst(node->as<IfExpr>().elifCondition[i], level + 1);
            printAst(node->as<IfExpr>().elifBlock[i], level + 1);
        }
        printAst(node->as<IfExpr>().elseBlock, level + 1);
    break;
    case NodeType::callArgsList:
        std::cout << std::endl;
        for(AstNode *args : node->as<CallArgsList>().args){
            printAst(args, level + 1);
        }
    break;
    case NodeType::arrayLiteral:
        std::cout << std::endl;
        for(AstNode *elem : node->as<ArrayLiteral>().elements){
            printAst(elem, level + 1);
        }
    break;
    case NodeType::arraySubscript:
        std::cout << std::endl;
        printAst(node->as<ArraySubscript>().index, level + 1);
    break;
    case NodeType::assignment:
        std::cout << std::endl;
        printAst(node->as<Assignment>().lhs, level + 1);
        printAst(node->as<Assignment>().rhs, level + 1);
    break;
    case NodeType::tuplePattern:
        std::cout << std::endl;
        for(AstNode *child : node->as<TuplePattern>().children){
            printAst(child, level + 1);
        }
    break;
    case NodeType::tupleExpression:
        std::cout << std::endl;
        for(AstNode *child : node->as<TupleExpression>().children){
            printAst(child, level + 1);
        }
    break;
    default:
        throw SystemError(std::string("printAst node type ") +
            getNodeTypeName(node->type) +  " is unimplemented", 
            __FILE_NAME__, __LINE__);
    }
}