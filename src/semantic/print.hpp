#include "../ast/astnode.hpp"
#include "../ast/operator.hpp"
#include "types.hpp"

#include <iostream>

static void printAAst(ExprNode *node, int level = 0){
    for(int i = 0; i < 4 * level; i++){
        std::cout << " ";
    }
    if(node == nullptr){
        std::cout << "[NULL]" << std::endl;
        return;
    }
    std::cout << getNodeTypeName(node->kind) << " : " << node->valueType->name;
    if(isOperator(node->kind)){
        std::cout << std::endl;
        if(isBinaryOperator(node->kind)){
            printAAst(node->as<BinaryOperation>().left, level + 1);
            printAAst(node->as<BinaryOperation>().right, level + 1);
        } else {
            printAAst(node->as<UnaryOperation>().expr, level + 1);
        }
        return;
    } 
    switch(node->kind){
    case ExprKind::identifier:
        std::cout << std::endl; 
    break;
    case ExprKind::typedIdentifier:
        std::cout << " | " << node->as<TypedIdentifier>().name << std::endl; 
    break;
    case ExprKind::intLiteral:
        std::cout << " | " << node->as<IntLiteral>().precomputed << std::endl; 
    break;
    case ExprKind::floatLiteral:
        std::cout << " | " << node->as<FloatLiteral>().precomputed << std::endl;
    break;
    case ExprKind::stringLiteral:
        std::cout << " | " << node->as<StringLiteral>().text << std::endl;
    break;
    case ExprKind::formatString:
        std::cout << std::endl;
        for(ExprNode *child : node->as<FormatString>().children){
            printAAst(child, level + 1);
        }
    break;
    case ExprKind::stringTemplate:
        std::cout << std::endl;
        printAAst(node->as<StringTemplate>().value, level + 1);
        printAAst(node->as<StringTemplate>().format, level + 1);
    break;
    case ExprKind::boolLiteral:
        std::cout << " | " << node->as<BoolLiteral>().text << std::endl;
    break;
    case ExprKind::fnParamList:
        std::cout << std::endl;
        for(ExprNode *param : node->as<FnParamList>().params){
            printAAst(param, level + 1);
        }
    break;
    case ExprKind::function:
        std::cout << " | " << node->as<Function>().name << std::endl;
        printAAst(node->as<Function>().paramList, level + 1);
        printAAst(node->as<Function>().block, level + 1);
    break;
    case ExprKind::block:
         std::cout << std::endl;
        for(ExprNode *expr : node->as<Block>().expressions){
            printAAst(expr, level + 1);
        }   
    break;
    case ExprKind::ifExpr:
        std::cout << std::endl;
        printAAst(node->as<IfExpr>().condition, level + 1);
        printAAst(node->as<IfExpr>().ifBlock, level + 1);
        for(int i = 0; i < node->as<IfExpr>().elifBlock.size(); i++){
            printAAst(node->as<IfExpr>().elifCondition[i], level + 1);
            printAAst(node->as<IfExpr>().elifBlock[i], level + 1);
        }
        printAAst(node->as<IfExpr>().elseBlock, level + 1);
    break;
    case ExprKind::callArgsList:
        std::cout << std::endl;
        for(ExprNode *args : node->as<CallArgsList>().args){
            printAAst(args, level + 1);
        }
    break;
    case ExprKind::arrayLiteral:
        std::cout << std::endl;
        for(ExprNode *elem : node->as<ArrayLiteral>().elements){
            printAAst(elem, level + 1);
        }
    break;
    case ExprKind::arraySubscript:
        std::cout << std::endl;
        printAAst(node->as<ArraySubscript>().index, level + 1);
    break;
    case ExprKind::assignment:
        std::cout << std::endl;
        printAAst(node->as<Assignment>().lhs, level + 1);
        printAAst(node->as<Assignment>().rhs, level + 1);
    break;
    case ExprKind::tuplePattern:
        std::cout << std::endl;
        for(ExprNode *child : node->as<TuplePattern>().children){
            printAAst(child, level + 1);
        }
    break;
    case ExprKind::tupleExpression:
        std::cout << std::endl;
        for(ExprNode *child : node->as<TupleExpression>().children){
            printAAst(child, level + 1);
        }
    break;
    case ExprKind::castToInt:
        std::cout << std::endl;
        printAAst(node->as<CastToInt>().expr, level + 1);
    break;
    case ExprKind::castToFloat:
        std::cout << std::endl;
        printAAst(node->as<CastToFloat>().expr, level + 1);
    break;
    case ExprKind::type:
        std::cout << std::endl;
        printAAst(node->as<TypeNode>().base, level + 1);
    break;
    case ExprKind::structure:
        std::cout << " | " << node->as<Structure>().name << std::endl;
        for(ExprNode *field : node->as<Structure>().fields){
            printAAst(field, level + 1);
        }
    break;
    default:
        throw SystemError(std::string("printAAst node type ") +
            getNodeTypeName(node->kind) +  " is unimplemented", 
            __FILE_NAME__, __LINE__);
    }
}