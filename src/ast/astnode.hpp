#pragma once

#include "../include/utils.hpp"
#include "../token/token.hpp"

#include <variant>

enum class NodeType {
    // Building blocks
    expression, block,
    // Primaries (*not exhaustive)
    identifier, intLiteral, floatLiteral, stringLiteral, formatString, stringTemplate,
    trueLiteral, falseLiteral,
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
    castToInt, castToFloat,
    type,
    structure, enumeration,
    matchExpr,
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
    { NodeType::trueLiteral, "trueLiteral" },
    { NodeType::falseLiteral, "falseLiteral" },
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
    { NodeType::type, "type" },
    { NodeType::structure, "structure" },
    { NodeType::enumeration, "enumeration" },
    { NodeType::matchExpr, "matchExpr" },
};

class AstNode;
class Type;

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
    std::string text;
    int precomputed;
};

struct StringLiteral {
    std::string text;
};

struct StringTemplate {
    AstNode *value;
    AstNode *format;
};

struct FormatString{
    std::vector<AstNode*> children;
};

struct TrueLiteral {
};

struct FalseLiteral {
};

struct FloatLiteral {
    std::string text;
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
    std::string name;
    AstNode *paramList;
    AstNode *returnType;
    AstNode *block;
};

struct TypeNode {
    AstNode* base;
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

struct Structure {
    std::string name;
    std::vector<AstNode*> fields;
    std::vector<AstNode*> methods;
};

struct Enumeration {

};

struct MatchExpr {

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
        TrueLiteral,
        FalseLiteral,
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
        CastToFloat,
        TypeNode,
        Structure,
        Enumeration,
        MatchExpr
    > data;
    Type *valueType;
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
    case NodeType::identifier: return new AstNode(type, Identifier{});
    case NodeType::typedIdentifier: return new AstNode(type, TypedIdentifier{});
    case NodeType::intLiteral: return new AstNode(type, IntLiteral{});
    case NodeType::floatLiteral: return new AstNode(type, FloatLiteral{});
    case NodeType::stringLiteral: return new AstNode(type, StringLiteral{});
    case NodeType::stringTemplate: return new AstNode(type, StringTemplate{});
    case NodeType::formatString: return new AstNode(type, FormatString{});
    case NodeType::trueLiteral: return new AstNode(type, TrueLiteral{});
    case NodeType::falseLiteral: return new AstNode(type, FalseLiteral{});
    case NodeType::addition: return new AstNode(type, BinaryOperation{});
    case NodeType::subtraction: return new AstNode(type, BinaryOperation{});
    case NodeType::multiplication: return new AstNode(type, BinaryOperation{});
    case NodeType::division: return new AstNode(type, BinaryOperation{});
    case NodeType::exponentiation: return new AstNode(type, BinaryOperation{});
    case NodeType::plusSign: return new AstNode(type, UnaryOperation{});
    case NodeType::minusSign: return new AstNode(type, UnaryOperation{});
    case NodeType::assignment: return new AstNode(type, Assignment{});
    case NodeType::conjunction: return new AstNode(type, BinaryOperation{});
    case NodeType::disjunction: return new AstNode(type, BinaryOperation{});
    case NodeType::negation: return new AstNode(type, UnaryOperation{});
    case NodeType::equality: return new AstNode(type, BinaryOperation{});
    case NodeType::inequality: return new AstNode(type, BinaryOperation{});
    case NodeType::lessThan: return new AstNode(type, BinaryOperation{});
    case NodeType::greaterThan: return new AstNode(type, BinaryOperation{});
    case NodeType::lessEqual: return new AstNode(type, BinaryOperation{});
    case NodeType::greaterEqual: return new AstNode(type, BinaryOperation{});
    case NodeType::memberAccess: return new AstNode(type, BinaryOperation{});
    case NodeType::function: return new AstNode(type, Function{});
    case NodeType::type: return new AstNode(type, TypeNode{});
    case NodeType::fnParamList: return new AstNode(type, FnParamList{});
    case NodeType::block: return new AstNode(type, Block{});
    case NodeType::ifExpr: return new AstNode(type, IfExpr{});
    case NodeType::call: return new AstNode(type, BinaryOperation{});
    case NodeType::callArgsList: return new AstNode(type, CallArgsList{});
    case NodeType::forExpr: return new AstNode(type, ForExpr{});
    case NodeType::arrayLiteral: return new AstNode(type, ArrayLiteral{});
    case NodeType::arrayAccess: return new AstNode(type, BinaryOperation{});
    case NodeType::arraySubscript: return new AstNode(type, ArraySubscript{});
    case NodeType::tuplePattern: return new AstNode(type, TuplePattern{});
    case NodeType::tupleExpression: return new AstNode(type, TupleExpression{});
    case NodeType::castToInt: return new AstNode(type, CastToInt{});
    case NodeType::castToFloat: return new AstNode(type, CastToFloat{});
    case NodeType::structure: return new AstNode(type, Structure{});
    case NodeType::enumeration: return new AstNode(type, Enumeration{});
    case NodeType::matchExpr: return new AstNode(type, MatchExpr{});
    default: throw SystemError("createNode unimplemented", __FILE_NAME__, __LINE__);
    }
}