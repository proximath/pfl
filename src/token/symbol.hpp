#pragma once

#include "token.hpp"

static std::unordered_map<std::string, TokenType> symbolLookup = {
    { "==", TokenType::doubleEqual },
    { "=",  TokenType::equal },
    { "!=", TokenType::notEqual },
    { "!",  TokenType::exclamation },
    { "+",  TokenType::plus },
    { "-",  TokenType::minus },
    { "**", TokenType::exponent },
    { "*",  TokenType::asterisk },
    { "//", TokenType::root },
    { "/",  TokenType::slash },
    { "&",  TokenType::ampersand },
    { "|",  TokenType::bar },
    { "&&", TokenType::doubleAmpersand },
    { "||", TokenType::doubleBar },
    { "<=", TokenType::lessEqual },
    { "<",  TokenType::less },
    { ">=", TokenType::moreEqual },
    { ">",  TokenType::more },
    { ",",  TokenType::comma },
    { ".",  TokenType::dot },
    { ":",  TokenType::colon },
    { ";",  TokenType::semicolon },
    { "(",  TokenType::parenStart },
    { ")",  TokenType::parenEnd },
    { "{",  TokenType::curlyStart },
    { "}",  TokenType::curlyEnd },
    { "[",  TokenType::squareStart },
    { "]",  TokenType::squareEnd },
    { "\'", TokenType::quote },
    { "\"", TokenType::doubleQuote },
    { "`",  TokenType::backTick }
};

static bool isSymbol(const std::string &prefix){
    return symbolLookup.count(prefix);
}

static bool isSymbol(char c){
    return symbolLookup.count(std::string(1, c));
}

static TokenType symbolToTokenType(const std::string &sym){
    return symbolLookup[sym];
}