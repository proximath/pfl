#pragma once

#include "token.hpp"

static std::unordered_set<TokenType> operatorLookup = {
    TokenType::plus,
    TokenType::minus,
    TokenType::asterisk,
    TokenType::slash,
    TokenType::exponent,
    TokenType::root,
    TokenType::doubleAmpersand,
    TokenType::doubleBar,
    TokenType::equal,
    TokenType::doubleEqual,
    TokenType::notEqual,
    TokenType::less,
    TokenType::lessEqual,
    TokenType::more,
    TokenType::moreEqual,
    TokenType::dot,
    TokenType::notKeyword,
    TokenType::andKeyword,
    TokenType::orKeyword,
    TokenType::isKeyword,
    TokenType::inKeyword,
};

static bool isOperator(const TokenType type){
    return operatorLookup.count(type);
}

static bool isOperator(const Token &token){
    return isOperator(token.type);
}

static bool isPostfixOp(const TokenType tt){
    return false;
}