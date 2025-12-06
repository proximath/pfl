#pragma once

#include "../include/utils.hpp"
#include "../token/token.hpp"

#include <variant>

enum class NodeType {
    // Building blocks
    expression, block,
    // Primaries (*not exhaustive)
    identifier, intLiteral, floatLiteral, stringLiteral,
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
};

static std::unordered_map<NodeType, const char*> nodeTypeNameLookup = {
    { NodeType::expression, "expression" },
    { NodeType::identifier, "identifier" },
    { NodeType::typedIdentifier, "typedIdentifier" },
    { NodeType::intLiteral, "intLiteral" },
    { NodeType::floatLiteral, "floatLiteral" },
    { NodeType::stringLiteral, "stringLiteral" },
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

};

class AstNode;

struct BinaryOperation {
    AstNode *left;
    AstNode *right;
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
};

struct StringLiteral {
    std::string value;
};

struct FloatLiteral {
    std::string value;
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

struct AstNode {
    NodeType type;
    std::variant<
        Identifier,
        TypedIdentifier,
        IntLiteral,
        FloatLiteral,
        StringLiteral,
        BinaryOperation,
        UnaryOperation,
        VariableDeclaration,
        Function,
        FnParamList,
        Block,
        IfExpr,
        CallArgsList,
        ArrayLiteral,
        Assignment,
        TuplePattern,
        TupleExpression,
        ArraySubscript
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