#pragma once

#include "../include/utils.hpp"
#include "../token/token.hpp"

#include <variant>

enum class NodeType {
    // Building blocks
    expression, block,
    // Primaries (*not exhaustive)
    identifier, intLiteral, floatLiteral, stringLiteral, formatString, stringTemplate,
    // Binary Arithmetic Operation
    addition, subtraction, multiplication, division, exponentiation,
    // Unary Operation
    plusSign, minusSign,
    assignment,
    // Logical Operation
    conjunction, disjunction, negation,
    // Comparisan
    equality, inequality, lessThan, greaterThan, lessEqual, greaterEqual,
    // Function
    function, fnParamList, callArgsList, call,
    // Statement-like
    ifExpr, forExpr,
    memberAccess,
    arrayLiteral, arrayAccess, arraySubscript,
    tuplePattern,
    tupleExpression,
    typedIdentifier,
    castToInt, castToFloat
};

static std::unordered_map<NodeType, const char*> nodeTypeNameLookup = {
    { NodeType::expression, "expression" },
    { NodeType::identifier, "identifier" },
    { NodeType::typedIdentifier, "typedIdentifier" },
    { NodeType::intLiteral, "intLiteral" },
    { NodeType::floatLiteral, "floatLiteral" },
    { NodeType::stringLiteral, "stringLiteral" },
    { NodeType::stringTemplate, "stringTemplate" },
    { NodeType::formatString, "formatString" },
    { NodeType::addition, "addition" },
    { NodeType::subtraction, "subtraction" },
    { NodeType::multiplication, "multiplication" },
    { NodeType::division, "division" },
    { NodeType::exponentiation, "exponentiation" },
    { NodeType::plusSign, "plusSign" },
    { NodeType::minusSign, "minusSign" },
    { NodeType::assignment, "assignment" },
    { NodeType::conjunction, "conjunction" },
    { NodeType::disjunction, "disjunction" },
    { NodeType::negation, "negation" },
    { NodeType::equality, "equality" },
    { NodeType::inequality, "inequality" },
    { NodeType::lessThan, "lessThan" },
    { NodeType::greaterThan, "greaterThan" },
    { NodeType::lessEqual, "lessEqual" },
    { NodeType::greaterEqual, "greaterEqual" },
    { NodeType::memberAccess, "memberAccess" },
    { NodeType::function, "function" },
    { NodeType::fnParamList, "fnParamList" },
    { NodeType::block, "block" },
    { NodeType::ifExpr, "ifExpr" },
    { NodeType::call, "call" },
    { NodeType::callArgsList, "callArgsList" },
    { NodeType::forExpr, "forExpr" },
    { NodeType::arrayLiteral, "arrayLiteral" },
    { NodeType::arrayAccess, "arrayAccess" },
    { NodeType::arraySubscript, "arraySubscript" },
    { NodeType::tuplePattern, "tuplePattern" },
    { NodeType::tupleExpression, "tupleExpression" },
    { NodeType::castToInt, "castToInt" },
    { NodeType::castToFloat, "castToFloat" },
};

class AstNode;
class Type;

struct BinaryOperation {
    AstNode *left;
    AstNode *right;
    Type *type;
};

struct UnaryOperation {
    AstNode *expr;
};

struct VariableDeclaration {
    AstNode *variableName;
    AstNode *typeName;
    AstNode *value;
};

struct IntLiteral {
    std::string value;
    int precomputed;
};

struct StringLiteral {
    std::string value;
};

struct StringTemplate {
    AstNode *value;
    AstNode *format;
};

struct FormatString{
    std::vector<AstNode*> children;
};

struct FloatLiteral {
    std::string value;
    double precomputed;
};

struct Identifier {
    std::string name;
};

struct TypedIdentifier {
    std::string name;
    std::string type; // Be careful if you decided to add Type<T> later
};

struct FnParamList {
    std::vector<AstNode*> params;
};

struct Function {
    AstNode *paramList;
    AstNode *name;
    AstNode *block;
};

struct Block {
    std::vector<AstNode*> expressions;
};

struct IfExpr {
    AstNode *condition;
    AstNode *ifBlock;
    std::vector<AstNode*> elifCondition;
    std::vector<AstNode*> elifBlock;
    AstNode *elseBlock;
};

struct ForExpr {
    AstNode *pattern;
    AstNode *expr;
    AstNode *block;
};

struct ArrayLiteral {
    std::vector<AstNode*> elements;
};

struct Assignment {
    AstNode *lhs;
    AstNode *rhs;
};

struct TuplePattern {
    std::vector<AstNode*> children;
};

struct TupleExpression {
    std::vector<AstNode*> children;
};

struct CallArgsList {
    std::vector<AstNode*> args;
};

struct ArraySubscript {
    AstNode *index;
};

struct CastToInt {
    AstNode *expr;
};

struct CastToFloat {
    AstNode *expr;
};

struct AstNode {
    NodeType type;
    std::variant<
        Identifier,
        TypedIdentifier,
        IntLiteral,
        FloatLiteral,
        StringLiteral,
        FormatString,
        StringTemplate,
        BinaryOperation,
        UnaryOperation,
        VariableDeclaration,
        Function,
        FnParamList,
        Block,
        IfExpr,
        ForExpr,
        CallArgsList,
        ArrayLiteral,
        Assignment,
        TuplePattern,
        TupleExpression,
        ArraySubscript,
        CastToInt,
        CastToFloat
    > data;
    AstNode(){}
    AstNode(NodeType type)
      : type(type) 
    {
    }
    template<typename T>
    AstNode(NodeType type, const T &data)
      : type(type)
    {
        this->data = data;
    }
    template<typename T>
    T& as(){
        return std::get<T>(data);
    }
};

static const char* getNodeTypeName(NodeType type){
    if(!nodeTypeNameLookup.count(type)){
        throw SystemError("getNodeTypeName not implemented", __FILE__, __LINE__);
    }
    return nodeTypeNameLookup[type];
}

static AstNode* createNode(NodeType type){
    switch(type){
    case NodeType::identifier:
        return new AstNode(type, Identifier{});
    break;
    case NodeType::typedIdentifier:
        return new AstNode(type, TypedIdentifier{});
    break;
    case NodeType::intLiteral:
        return new AstNode(type, IntLiteral{});
    break;
    case NodeType::floatLiteral:
        return new AstNode(type, FloatLiteral{});
    break;
    case NodeType::stringLiteral:
        return new AstNode(type, StringLiteral{});
    break;
    case NodeType::stringTemplate:
        return new AstNode(type, StringTemplate{});
    break;
    case NodeType::formatString:
        return new AstNode(type, FormatString{});
    break;
    case NodeType::addition:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::subtraction:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::multiplication:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::division:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::exponentiation:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::plusSign:
        return new AstNode(type, UnaryOperation{});
    break;
    case NodeType::minusSign:
        return new AstNode(type, UnaryOperation{});
    break;
    case NodeType::assignment:
        return new AstNode(type, Assignment{});
    break;
    case NodeType::conjunction:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::disjunction:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::negation:
        return new AstNode(type, UnaryOperation{});
    break;
    case NodeType::equality:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::inequality:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::lessThan:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::greaterThan:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::lessEqual:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::greaterEqual:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::memberAccess:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::function:
        return new AstNode(type, Function{});
    break;
    case NodeType::fnParamList:
        return new AstNode(type, FnParamList{});
    break;
    case NodeType::block:
        return new AstNode(type, Block{});
    break;
    case NodeType::ifExpr:
        return new AstNode(type, IfExpr{});
    break;
    case NodeType::call:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::callArgsList:
        return new AstNode(type, CallArgsList{});
    break;
    case NodeType::forExpr:
        return new AstNode(type, ForExpr{});
    break;
    case NodeType::arrayLiteral:
        return new AstNode(type, ArrayLiteral{});
    break;
    case NodeType::arrayAccess:
        return new AstNode(type, BinaryOperation{});
    break;
    case NodeType::arraySubscript:
        return new AstNode(type, ArraySubscript{});
    break;
    case NodeType::tuplePattern:
        return new AstNode(type, TuplePattern{});
    break;
    case NodeType::tupleExpression:
        return new AstNode(type, TupleExpression{});
    break;
    case NodeType::castToInt:
        return new AstNode(type, CastToInt{});
    break;
    case NodeType::castToFloat:
        return new AstNode(type, CastToFloat{});
    break;
    default:
        throw SystemError("createNode unimplemented", __FILE_NAME__, __LINE__);
    }
}