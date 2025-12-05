#pragma once

#include "token.hpp"

static std::unordered_map<std::string, TokenType> keywordLookup = {
    { "fn", TokenType::fnKeyword },
    { "for", TokenType::forKeyword },
    { "if", TokenType::ifKeyword },
    { "else", TokenType::elseKeyword },
    { "elif", TokenType::elifKeyword },
    { "do", TokenType::doKeyword },
    { "true", TokenType::trueKeyword },
    { "false", TokenType::falseKeyword },
    { "type", TokenType::typeKeyword },
    { "not", TokenType::notKeyword },
    { "and", TokenType::andKeyword },
    { "or", TokenType::orKeyword },
    { "in", TokenType::inKeyword },
    { "is", TokenType::isKeyword },
};

static bool isKeyword(const std::string &word){
    return keywordLookup.count(word);
}

static TokenType keywordToTokenType(const std::string &word){
    return keywordLookup[word];
}