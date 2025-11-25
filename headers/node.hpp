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
        default:
            throw SystemError("getNodeTypeName trying to get name of unimplemented node type", __FILE_NAME__, __LINE__);
    }
}

static std::unordered_map<NodeType, std::pair<int, int>> bindingPowerLookup = {
    { NodeType::addition, { 11, 10 } },
    { NodeType::subtraction, { 11, 10 } },
    { NodeType::multiplication, { 21, 20 } },
    { NodeType::division, { 21, 20 } },
    { NodeType::exponentiation, { 31, 30 } },
    { NodeType::plusSign, { 0, 100 } },
    { NodeType::minusSign, { 0, 100 } }
};

static bool isPrimary(NodeType type){
    return type == NodeType::identifier ||
    type == NodeType::intLiteral ||
    type == NodeType::floatLiteral ||
    type == NodeType::stringLiteral;
}

static bool isOperator(NodeType type){
    return type == NodeType::addition ||
    type == NodeType::subtraction ||
    type == NodeType::exponentiation ||
    type == NodeType::multiplication ||
    type == NodeType::division ||
    type == NodeType::plusSign ||
    type == NodeType::minusSign;
}

static int getLbp(NodeType type){
    if(isPrimary(type)){
        return 0;
    }
    if(!bindingPowerLookup.count(type)){
        throw SystemError("Binding power not found", __FILE_NAME__, __LINE__);
    }
    return bindingPowerLookup[type].first;
}

static int getRbp(NodeType type){
    if(isPrimary(type)){
        return 0;
    }
    if(!bindingPowerLookup.count(type)){
        throw SystemError("Binding power not found", __FILE_NAME__, __LINE__);
    }
    return bindingPowerLookup[type].second;
}

class AstNode;

struct Identifier {
    std::string name;
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
        Identifier,
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

