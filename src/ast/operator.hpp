#pragma once

#include "astnode.hpp"

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