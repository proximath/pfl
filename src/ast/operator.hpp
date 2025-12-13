#pragma once

#include "astnode.hpp"

struct OperatorInfo {
    int lbp, rbp;
    bool binary;
    bool prefix;
};

static std::unordered_map<ExprKind, OperatorInfo> operatorInfoLookup = {
    { ExprKind::addition, { 10, 11, true, false } },
    { ExprKind::subtraction, { 10, 11, true, false } },
    { ExprKind::multiplication, { 20, 21, true, false } },
    { ExprKind::division, { 20, 21, true, false } },
    { ExprKind::exponentiation, { 30, 31, true, false } },
    { ExprKind::plusSign, { 100, 101, false, true } },
    { ExprKind::minusSign, { 100, 101, false, true } },
    //{ ExprKind::assignment, { 1, 2, true, false } },
    { ExprKind::conjunction, { 3, 4, true, false } },
    { ExprKind::disjunction, { 3, 4, true, false } },
    { ExprKind::negation, { 0, 8, false, true } },
    { ExprKind::equality, { 5, 6, true, false } },
    { ExprKind::inequality, { 5, 6, true, false } },
    { ExprKind::lessThan, { 5, 6, true, false } },
    { ExprKind::greaterThan, { 5, 6, true, false } },
    { ExprKind::lessEqual, { 5, 6, true, false } },
    { ExprKind::greaterEqual, { 5, 6, true, false } },
    { ExprKind::memberAccess, { 200, 201, true, false } },
    { ExprKind::arrayAccess, { 150, 1000, true, false } },
    { ExprKind::call, {150, 1000, true, false } }
};

static bool isOperator(ExprKind type){
    return operatorInfoLookup.count(type);
}

static bool isBinaryOperator(ExprKind type){
    return operatorInfoLookup[type].binary;
}

static bool isUnaryOperator(ExprKind type){
    return !operatorInfoLookup[type].binary;
}

static bool isPrefixOperator(ExprKind type){
    return operatorInfoLookup[type].prefix;
}

static bool isPostfixOperator(ExprKind type){
    return !operatorInfoLookup[type].prefix;
}

static int getLbp(ExprKind type){
    if(!operatorInfoLookup.count(type)){
        throw SystemError(
            std::string("Binding power of ") + getNodeTypeName(type) + " not found", 
            __FILE_NAME__, __LINE__);
    }
    return operatorInfoLookup[type].lbp;
}

static int getRbp(ExprKind type){
    if(!operatorInfoLookup.count(type)){
        throw SystemError(
            std::string("Binding power of ") + getNodeTypeName(type) + " not found", 
            __FILE_NAME__, __LINE__);
    }
    return operatorInfoLookup[type].rbp;
}