#include "astnode.hpp"
#include "operator.hpp"

#include <iostream>

static void printAst(TypeNode *, int);
static void printAst(TuplePatternBase *, int);
static void printAst(StructMethod *, int);

static void printAst(ExprNode *node, int level = 0){
    printSpace(level);
    if(node == nullptr){
        std::cout << "[NULL]" << std::endl;
        return;
    }
    std::cout << getNodeTypeName(node->kind);
    if(isOperator(node->kind)){
        std::cout << std::endl;
        if(isBinaryOperator(node->kind)){
            printAst(node->as<BinaryOperation>().left, level + 1);
            printAst(node->as<BinaryOperation>().right, level + 1);
        } else {
            printAst(node->as<UnaryOperation>().expr, level + 1);
        }
        return;
    } 
    switch(node->kind){
    case ExprKind::identifier:
        std::cout << " | " << node->as<Identifier>().name << std::endl; 
    break;
    case ExprKind::typedIdentifier:
        std::cout << " | ";
        std::cout << node->as<TypedIdentifier>().name << " | "; 
        std::cout << node->as<TypedIdentifier>().type << std::endl; 
    break;
    case ExprKind::intLiteral:
        std::cout << " | " << node->as<IntLiteral>().text << std::endl; 
    break;
    case ExprKind::floatLiteral:
        std::cout << " | " << node->as<FloatLiteral>().text << std::endl;
    break;
    case ExprKind::stringLiteral:
        std::cout << " | " << node->as<StringLiteral>().text << std::endl;
    break;
    case ExprKind::formatString:
        std::cout << std::endl;
        for(ExprNode *child : node->as<FormatString>().children){
            printAst(child, level + 1);
        }
    break;
    case ExprKind::stringTemplate:
        std::cout << std::endl;
        printAst(node->as<StringTemplate>().value, level + 1);
        printAst(node->as<StringTemplate>().format, level + 1);
    break;
    case ExprKind::boolLiteral:
        std::cout << node->as<BoolLiteral>().text << std::endl;
    break;
    case ExprKind::function:
        std::cout << " | " << node->as<Function>().name << std::endl;
        printSpace(level + 1);
        std::cout << ".params:" << std::endl;
        for(FunctionParam *param : node->as<Function>().params){
            printSpace(level + 2);    
            std::cout << "functionParam | " << param->name << std::endl;
            printAst(param->type, level + 3);
        }
        printAst(node->as<Function>().returnType, level + 1);
        printAst(node->as<Function>().block, level + 1);
    break;
    case ExprKind::block:
         std::cout << std::endl;
        for(ExprNode *expr : node->as<Block>().expressions){
            printAst(expr, level + 1);
        }   
    break;
    case ExprKind::ifExpr:
        std::cout << std::endl;
        printAst(node->as<IfExpr>().condition, level + 1);
        printAst(node->as<IfExpr>().ifBlock, level + 1);
        for(int i = 0; i < node->as<IfExpr>().elifBlock.size(); i++){
            printAst(node->as<IfExpr>().elifCondition[i], level + 1);
            printAst(node->as<IfExpr>().elifBlock[i], level + 1);
        }
        printAst(node->as<IfExpr>().elseBlock, level + 1);
    break;
    case ExprKind::callArgsList:
        std::cout << std::endl;
        for(ExprNode *args : node->as<CallArgsList>().args){
            printAst(args, level + 1);
        }
    break;
    case ExprKind::arrayLiteral:
        std::cout << std::endl;
        for(ExprNode *elem : node->as<ArrayLiteral>().elements){
            printAst(elem, level + 1);
        }
    break;
    case ExprKind::arraySubscript:
        std::cout << std::endl;
        printAst(node->as<ArraySubscript>().index, level + 1);
    break;
    case ExprKind::assignment:
        std::cout << std::endl;
        printAst(node->as<Assignment>().lhs, level + 1);
        printAst(node->as<Assignment>().rhs, level + 1);
    break;
    case ExprKind::tupleExpression:
        std::cout << std::endl;
        for(ExprNode *child : node->as<TupleExpression>().children){
            printAst(child, level + 1);
        }
    break;
    case ExprKind::structure:
        std::cout << " | " << node->as<Structure>().name << std::endl;
        printSpace(level + 1);
        std::cout << ".generics:" << std::endl;
        for(std::string &field : node->as<Structure>().generics){
            printSpace(level + 2);
            std::cout << field << std::endl;
        }
        for(StructAttribute *attr : node->as<Structure>().attributes){
            printSpace(level + 1);
            std::cout << "structAttribute | " << attr->name << std::endl;
            printAst(attr->type, level + 2);
        }
        for(StructMethod *method : node->as<Structure>().methods){
            printAst(method, level + 1);
        }
    break;
    default:
        throw SystemError(std::string("printAst node kind ") +
            getNodeTypeName(node->kind) +  " is unimplemented", 
            __FILE_NAME__, __LINE__);
    }
}

static void printAst(TypeNode *type, int level){
    if(type == nullptr){
        return;
    }
    printSpace(level); 
    std::cout << "typeNode | " << type->main << std::endl;
    if(!type->generics.empty()){
        for(TypeNode *gen : type->generics){
            printAst(gen, level + 1);
        }
    }
}

static void printAst(TuplePatternBase *pattern, int level){
    if(pattern == nullptr){
        return;
    }
    printSpace(level);
    if(pattern->isLeaf){
        std::cout << "tuplePatternLeaf | ";
        std::cout << pattern->asLeaf()->name << std::endl;
        printAst(pattern->asLeaf()->type, level + 1);
    } else {
        std::cout << "tuplePattern" << std::endl;
        for(TuplePatternBase *child : pattern->asNode()->children){
            printAst(child, level + 1);
        }
    }
}

static void printAst(StructMethod *method, int level){
    if(method == nullptr){
        return;
    }
    printSpace(level);
    std::cout << "structMethod | " << method->name << std::endl;
    printSpace(level + 1);
    std::cout << ".params:" << std::endl;
    for(FunctionParam *param : method->params){
        printSpace(level + 2);    
        std::cout << "functionParam | " << param->name << std::endl;
        printAst(param->type, level + 3);
    }
    printAst(method->returnType, level + 1);
    printAst(method->block, level + 1);
}