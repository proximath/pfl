#include "../headers/utils.hpp"
#include "../headers/token.hpp"

#include <variant>

enum class NodeType {
    none,
    expression,
    primary,
    identifier,
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
    equality,
    inequality,
    lessThan,
    greaterThan,
    lessEqual,
    greaterEqual,
};

static const char* getNodeTypeName(NodeType type){
    switch(type){
        case NodeType::expression:
            return "expression";
        case NodeType::primary:
            return "primary";
        case NodeType::identifier:
            return "identifier";
        case NodeType::intLiteral:
            return "intLiteral";
        case NodeType::floatLiteral:
            return "floatLiteral";
        case NodeType::stringLiteral:
            return "stringLiteral";
        case NodeType::addition:
            return "addition";
        case NodeType::subtraction:
            return "subtraction";
        case NodeType::multiplication:
            return "multiplication";
        case NodeType::division:
            return "division";
        case NodeType::exponentiation:
            return "exponentiation";
        case NodeType::plusSign:
            return "plusSign";
        case NodeType::minusSign:
            return "minusSign";
        case NodeType::assignment:
            return "assignment";
        case NodeType::equality:
            return "equality";
        case NodeType::inequality:
            return "inequality";
        case NodeType::lessThan:
            return "lessThan";
        case NodeType::greaterThan:
            return "greaterThan";
        case NodeType::lessEqual:
            return "lessEqual";
        case NodeType::greaterEqual:
            return "greaterEqual";
        default:
            throw SystemError("getNodeTypeName trying to get name of unimplemented node type", __FILE_NAME__, __LINE__);
    }
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
    { NodeType::assignment, { 3, 4, true, false } },
    { NodeType::equality, { 5, 6, true, false } },
    { NodeType::inequality, { 5, 6, true, false } },
    { NodeType::lessThan, { 5, 6, true, false } },
    { NodeType::greaterThan, { 5, 6, true, false } },
    { NodeType::lessEqual, { 5, 6, true, false } },
    { NodeType::greaterEqual, { 5, 6, true, false } },
};

static bool isPrimary(NodeType type){
    return type == NodeType::identifier ||
    type == NodeType::intLiteral ||
    type == NodeType::floatLiteral ||
    type == NodeType::stringLiteral;
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
    if(isPrimary(type)){
        return 1000000;
    }
    if(!operatorInfoLookup.count(type)){
        throw SystemError("Binding power not found", __FILE_NAME__, __LINE__);
    }
    return operatorInfoLookup[type].lbp;
}

static int getRbp(NodeType type){
    if(isPrimary(type)){
        return 0;
    }
    if(!operatorInfoLookup.count(type)){
        throw SystemError("Binding power not found", __FILE_NAME__, __LINE__);
    }
    return operatorInfoLookup[type].rbp;
}

class AstNode;

struct Primary {
    std::string text;
};

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

struct AstNode {
    NodeType type;
    std::variant<
        Primary,
        BinaryOperation,
        UnaryOperation,
        VariableDeclaration
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

