#pragma once

#include "../include/utils.hpp"
#include "../token/token.hpp"

#include <variant>

enum  ExprKind {
    // Building blocks
    block,
    // Primaries (*not exhaustive)
    identifier, intLiteral, floatLiteral, stringLiteral, formatString, stringTemplate,
    boolLiteral,
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
    function, callArgsList, call,
    // Statement-like
    ifExpr, forExpr,
    memberAccess,
    arrayLiteral, arrayAccess, arraySubscript,
    tupleExpression,
    typedIdentifier,
    castToInt, castToFloat,
    structure, enumeration,
    matchExpr,
};

static std::unordered_map<ExprKind, const char*> nodeTypeNameLookup = {
    { ExprKind::identifier, "identifier" },
    { ExprKind::typedIdentifier, "typedIdentifier" },
    { ExprKind::intLiteral, "intLiteral" },
    { ExprKind::floatLiteral, "floatLiteral" },
    { ExprKind::stringLiteral, "stringLiteral" },
    { ExprKind::stringTemplate, "stringTemplate" },
    { ExprKind::formatString, "formatString" },
    { ExprKind::boolLiteral, "boolLiteral" },
    { ExprKind::addition, "addition" },
    { ExprKind::subtraction, "subtraction" },
    { ExprKind::multiplication, "multiplication" },
    { ExprKind::division, "division" },
    { ExprKind::exponentiation, "exponentiation" },
    { ExprKind::plusSign, "plusSign" },
    { ExprKind::minusSign, "minusSign" },
    { ExprKind::assignment, "assignment" },
    { ExprKind::conjunction, "conjunction" },
    { ExprKind::disjunction, "disjunction" },
    { ExprKind::negation, "negation" },
    { ExprKind::equality, "equality" },
    { ExprKind::inequality, "inequality" },
    { ExprKind::lessThan, "lessThan" },
    { ExprKind::greaterThan, "greaterThan" },
    { ExprKind::lessEqual, "lessEqual" },
    { ExprKind::greaterEqual, "greaterEqual" },
    { ExprKind::memberAccess, "memberAccess" },
    { ExprKind::function, "function" },
    { ExprKind::block, "block" },
    { ExprKind::ifExpr, "ifExpr" },
    { ExprKind::call, "call" },
    { ExprKind::callArgsList, "callArgsList" },
    { ExprKind::forExpr, "forExpr" },
    { ExprKind::arrayLiteral, "arrayLiteral" },
    { ExprKind::arrayAccess, "arrayAccess" },
    { ExprKind::arraySubscript, "arraySubscript" },
    { ExprKind::tupleExpression, "tupleExpression" },
    { ExprKind::castToInt, "castToInt" },
    { ExprKind::castToFloat, "castToFloat" },
    { ExprKind::structure, "structure" },
    { ExprKind::enumeration, "enumeration" },
    { ExprKind::matchExpr, "matchExpr" },
};

class ExprNode;
class Type;

enum class TypeNodeKind {
    atom,
    function,
    array,
    option,
    result,
    tuple
};

struct TypeNode {
    TypeNodeKind kind;
    template<typename T>
    T* pun(){
        return reinterpret_cast<T*>(this);
    }
    template<typename T>
    T& as(){
        return *reinterpret_cast<T*>(this);
    }
};

struct TypeNodeFunction : TypeNode {
    std::vector<TypeNode*> args;
    TypeNode *retType;
};

struct TypeNodeArray : TypeNode {
    TypeNode *elemType;
};

struct TypeNodeAtom : TypeNode {
    std::string main;
    std::vector<TypeNode*> generics;
};

struct TypeNodeOption : TypeNode {
    TypeNode *innerType;
};

struct TypeNodeResult : TypeNode {
    TypeNode *innerType;
};

struct TypeNodeTuple : TypeNode {
    std::vector<TypeNode*> children;
};

struct BinaryOperation {
    ExprNode *left;
    ExprNode *right;
};

struct UnaryOperation {
    ExprNode *expr;
};

struct VariableDeclaration {
    ExprNode *variableName;
    ExprNode *typeName;
    ExprNode *value;
};

struct IntLiteral {
    std::string text;
    int precomputed;
};

struct StringLiteral {
    std::string text;
};

struct StringTemplate {
    ExprNode *value;
    ExprNode *format;
};

struct FormatString{
    std::vector<ExprNode*> children;
};

struct BoolLiteral {
    std::string text;
    double precomputed;
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

struct FunctionParam {
    std::string name;
    TypeNode *type;
};

struct Function {
    std::string name;
    std::vector<FunctionParam*> params;
    TypeNode *returnType;
    ExprNode *block;
};

struct Block {
    std::vector<ExprNode*> expressions;
};

struct IfExpr {
    ExprNode *condition;
    ExprNode *ifBlock;
    std::vector<ExprNode*> elifCondition;
    std::vector<ExprNode*> elifBlock;
    ExprNode *elseBlock;
};

struct ArrayLiteral {
    std::vector<ExprNode*> elements;
};

struct TuplePatternLeaf;
struct TuplePatternNode;

struct TuplePatternBase {
    bool isLeaf = false;
    TuplePatternNode* asNode(){
        return reinterpret_cast<TuplePatternNode*>(this);
    }
    TuplePatternLeaf* asLeaf(){
        return reinterpret_cast<TuplePatternLeaf*>(this);
    }
};

struct TuplePatternLeaf : TuplePatternBase{
    std::string name;
    TypeNode *type;
};

struct TuplePatternNode : TuplePatternBase {
    std::vector<TuplePatternBase*> children;
};

struct Assignment {
    TuplePatternBase *lhs;
    ExprNode *rhs;
};

struct TupleExpression {
    std::vector<ExprNode*> children;
};

struct CallArgsList {
    std::vector<ExprNode*> args;
};

struct KeywordCallArgsList {
    std::vector<std::string> names;
    std::vector<ExprNode*> values;
};

struct ArraySubscript {
    ExprNode *index;
};

struct CastToInt {
    ExprNode *expr;
};

struct CastToFloat {
    ExprNode *expr;
};

struct StructAttribute {
    std::string name;
    TypeNode *type;
    ExprNode *defaultValue;
    bool isPublic;
};

struct StructMethod {
    std::string name;
    std::vector<FunctionParam*> params;
    TypeNode* returnType = nullptr;
    ExprNode* block;
    bool isStatic = true;
};

struct Structure {
    std::string name;
    std::vector<StructAttribute*> attributes;
    std::vector<std::string> generics;
    std::vector<StructMethod*> methods;
};

struct EnumVariant {
    std::string name;
    TypeNode *type;
};

struct Enumeration {
    std::string name;
    std::vector<EnumVariant*> variants;
    std::vector<std::string> generics;
    std::vector<StructMethod*> methods;
};

struct MatchExpr {
    ExprNode *expr;
    std::vector<std::string> variants;
    std::vector<TuplePatternBase*> patterns;
    std::vector<ExprNode*> blocks;
};

struct ExprNode {
    ExprKind kind;
    std::variant<
        Identifier,
        TypedIdentifier,
        IntLiteral,
        FloatLiteral,
        StringLiteral,
        FormatString,
        StringTemplate,
        BoolLiteral,
        BinaryOperation,
        UnaryOperation,
        VariableDeclaration,
        Function,
        Block,
        IfExpr,
        CallArgsList,
        ArrayLiteral,
        Assignment,
        TupleExpression,
        ArraySubscript,
        CastToInt,
        CastToFloat,
        Structure,
        Enumeration,
        MatchExpr
    > data;
    Type *valueType;
    template<typename T>
    ExprNode(ExprKind type, const T &data)
      : kind(type)
    {
        this->data = data;
    }
    template<typename T>
    T& as(){
        return std::get<T>(data);
    }
    template<typename T>
    T* pun(){
        return &std::get<T>(data);
    }
};

static const char* getNodeTypeName(ExprKind type){
    if(!nodeTypeNameLookup.count(type)){
        throw SystemError("getNodeTypeName not implemented", __FILE_NAME__, __LINE__);
    }
    return nodeTypeNameLookup[type];
}

static ExprNode* createNode(ExprKind type){
    switch(type){
    case ExprKind::identifier: return new ExprNode(type, Identifier{});
    case ExprKind::typedIdentifier: return new ExprNode(type, TypedIdentifier{});
    case ExprKind::intLiteral: return new ExprNode(type, IntLiteral{});
    case ExprKind::floatLiteral: return new ExprNode(type, FloatLiteral{});
    case ExprKind::stringLiteral: return new ExprNode(type, StringLiteral{});
    case ExprKind::stringTemplate: return new ExprNode(type, StringTemplate{});
    case ExprKind::formatString: return new ExprNode(type, FormatString{});
    case ExprKind::boolLiteral: return new ExprNode(type, BoolLiteral{});
    case ExprKind::addition: return new ExprNode(type, BinaryOperation{});
    case ExprKind::subtraction: return new ExprNode(type, BinaryOperation{});
    case ExprKind::multiplication: return new ExprNode(type, BinaryOperation{});
    case ExprKind::division: return new ExprNode(type, BinaryOperation{});
    case ExprKind::exponentiation: return new ExprNode(type, BinaryOperation{});
    case ExprKind::plusSign: return new ExprNode(type, UnaryOperation{});
    case ExprKind::minusSign: return new ExprNode(type, UnaryOperation{});
    case ExprKind::assignment: return new ExprNode(type, Assignment{});
    case ExprKind::conjunction: return new ExprNode(type, BinaryOperation{});
    case ExprKind::disjunction: return new ExprNode(type, BinaryOperation{});
    case ExprKind::negation: return new ExprNode(type, UnaryOperation{});
    case ExprKind::equality: return new ExprNode(type, BinaryOperation{});
    case ExprKind::inequality: return new ExprNode(type, BinaryOperation{});
    case ExprKind::lessThan: return new ExprNode(type, BinaryOperation{});
    case ExprKind::greaterThan: return new ExprNode(type, BinaryOperation{});
    case ExprKind::lessEqual: return new ExprNode(type, BinaryOperation{});
    case ExprKind::greaterEqual: return new ExprNode(type, BinaryOperation{});
    case ExprKind::memberAccess: return new ExprNode(type, BinaryOperation{});
    case ExprKind::function: return new ExprNode(type, Function{});
    case ExprKind::block: return new ExprNode(type, Block{});
    case ExprKind::ifExpr: return new ExprNode(type, IfExpr{});
    case ExprKind::call: return new ExprNode(type, BinaryOperation{});
    case ExprKind::callArgsList: return new ExprNode(type, CallArgsList{});
    case ExprKind::arrayLiteral: return new ExprNode(type, ArrayLiteral{});
    case ExprKind::arrayAccess: return new ExprNode(type, BinaryOperation{});
    case ExprKind::arraySubscript: return new ExprNode(type, ArraySubscript{});
    case ExprKind::tupleExpression: return new ExprNode(type, TupleExpression{});
    case ExprKind::castToInt: return new ExprNode(type, CastToInt{});
    case ExprKind::castToFloat: return new ExprNode(type, CastToFloat{});
    case ExprKind::structure: return new ExprNode(type, Structure{});
    case ExprKind::enumeration: return new ExprNode(type, Enumeration{});
    case ExprKind::matchExpr: return new ExprNode(type, MatchExpr{});
    default: throw SystemError("createNode unimplemented", __FILE_NAME__, __LINE__);
    }
}