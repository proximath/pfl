#include "../headers/utils.hpp"
#include "../headers/token.hpp"

#include <variant>

enum class NodeType {
    none,
    expression,
    primary,
    identifier,
    typedIdentifier,
    intLiteral,
    floatLiteral,
    stringLiteral,
    addition,
    subtraction,
    multiplication,
    division,
    exponentiation,
    plusSign,
    minusSign,
    assignment,
    conjunction,
    disjunction,
    negation,
    equality,
    inequality,
    lessThan,
    greaterThan,
    lessEqual,
    greaterEqual,
    function,
    fnParamList,
    block,
    ifExpr,
    call,
    callArgsList,
    memberAccess,
    forExpr,
    arrayLiteral,
    tuplePattern,
    tuplePatternLeaf,
};

static std::unordered_map<NodeType, const char*> nodeTypeNameLookup = {
    { NodeType::expression, "expression" },
    { NodeType::primary, "primary" },
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
    { NodeType::tuplePattern, "tuplePattern" },
    { NodeType::tuplePatternLeaf, "tuplePatternLeaf" },

};

static const char* getNodeTypeName(NodeType type){
    if(!nodeTypeNameLookup.count(type)){
        throw SystemError("getNodeTypeName not implemented", __FILE__, __LINE__);
    }
    return nodeTypeNameLookup[type];
}

struct OperatorInfo {
    int lbp, rbp;
    bool binary;
    bool prefix;
};

static std::unordered_map<NodeType, OperatorInfo> operatorInfoLookup = {
    { NodeType::addition, { 10, 11, true, false } },
    { NodeType::subtraction, { 10, 11, true, false } },
    { NodeType::multiplication, { 20, 21, true, false } },
    { NodeType::division, { 20, 21, true, false } },
    { NodeType::exponentiation, { 30, 31, true, false } },
    { NodeType::plusSign, { 100, 101, false, true } },
    { NodeType::minusSign, { 100, 101, false, true } },
    { NodeType::assignment, { 1, 2, true, false } },
    { NodeType::conjunction, { 3, 4, true, false } },
    { NodeType::disjunction, { 3, 4, true, false } },
    { NodeType::negation, { 0, 8, false, true } },
    { NodeType::equality, { 5, 6, true, false } },
    { NodeType::inequality, { 5, 6, true, false } },
    { NodeType::lessThan, { 5, 6, true, false } },
    { NodeType::greaterThan, { 5, 6, true, false } },
    { NodeType::lessEqual, { 5, 6, true, false } },
    { NodeType::greaterEqual, { 5, 6, true, false } },
    { NodeType::memberAccess, { 200, 201, true, false } },
    { NodeType::call, {150, 1000, true, false } }
};

static bool isPrimary(NodeType type){
    return 
    type == NodeType::identifier ||
    type == NodeType::intLiteral ||
    type == NodeType::floatLiteral ||
    type == NodeType::stringLiteral ||
    type == NodeType::callArgsList;
}

static bool isOperator(NodeType type){
    return operatorInfoLookup.count(type);
}

static bool isBinaryOperator(NodeType type){
    return operatorInfoLookup[type].binary;
}

static bool isUnaryOperator(NodeType type){
    return !operatorInfoLookup[type].binary;
}

static bool isPrefixOperator(NodeType type){
    return operatorInfoLookup[type].prefix;
}

static bool isPostfixOperator(NodeType type){
    return !operatorInfoLookup[type].prefix;
}

static int getLbp(NodeType type){
    if(!operatorInfoLookup.count(type)){
        throw SystemError("Binding power not found", __FILE_NAME__, __LINE__);
    }
    return operatorInfoLookup[type].lbp;
}

static int getRbp(NodeType type){
    if(!operatorInfoLookup.count(type)){
        throw SystemError("Binding power not found", __FILE_NAME__, __LINE__);
    }
    return operatorInfoLookup[type].rbp;
}

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
    AstNode *name;
    AstNode *type;
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

struct TuplePatternLeaf {
    AstNode *identifier;
};

struct TuplePattern {
    std::vector<AstNode*> children;
};

struct CallArgsList {
    std::vector<AstNode*> args;
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
        TuplePatternLeaf
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

