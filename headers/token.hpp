#pragma once

#include "utils.hpp"
#include <unordered_map>

enum class TokenType {
    placeholder, // Don't use
    identifier,
    keyword,
    intLiteral,
    floatLiteral,
    string,
    stringBegin,
    stringMiddle,
    stringEnd,
    plus,
    minus,
    asterisk,
    slash,
    exponent,
    root,
    equal,
    doubleEqual,
    less,
    lessEqual,
    more,
    moreEqual,
    comma,
    semicolon,
    colon,
    dot,
    exclamation,
    doubleAmpersand,
    ampersand,
    bar,
    doubleBar,
    parenStart,
    parenEnd,
    curlyStart,
    curlyEnd,
    squareStart,
    squareEnd,
    quote,
    doubleQuote,
    backTick,
    ifKeyword,
    forKeyword,
    doKeyword,
    fnKeyword
};

struct Token {
    int line_num;
    int column_num;
    std::string text;
    TokenType type;
};

static std::unordered_map<std::string_view, TokenType> symbolLookup = {
    { "==", TokenType::doubleEqual },
    { "=",  TokenType::equal },
    { "+",  TokenType::plus },
    { "-",  TokenType::minus },
    { "**", TokenType::exponent },
    { "*",  TokenType::asterisk },
    { "//", TokenType::root },
    { "/",  TokenType::slash },
    { "<=", TokenType::lessEqual },
    { "<",  TokenType::less },
    { ">=", TokenType::moreEqual },
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

static std::unordered_map<std::string_view, TokenType> keywordLookup = {
    { "fn", TokenType::fnKeyword },
    { "for", TokenType::forKeyword },
    { "if", TokenType::ifKeyword },
    { "do", TokenType::doKeyword },
};

static std::unordered_map<TokenType, std::string_view> tokenTypeNameLookup = {
    { TokenType::placeholder, "placeholder" },
    { TokenType::identifier, "identifier" },
    { TokenType::keyword, "keyword" },
    { TokenType::intLiteral, "intLiteral" },
    { TokenType::floatLiteral, "floatLiteral" },
    { TokenType::string, "string" },
    { TokenType::stringBegin, "stringBegin" },
    { TokenType::stringMiddle, "stringMiddle" },
    { TokenType::stringEnd, "stringEnd" },
    { TokenType::plus, "plus" },
    { TokenType::minus, "minus" },
    { TokenType::asterisk, "asterisk" },
    { TokenType::slash, "slash" },
    { TokenType::exponent, "exponent" },
    { TokenType::root, "root" },
    { TokenType::equal, "equal" },
    { TokenType::doubleEqual, "doubleEqual" },
    { TokenType::less, "less" },
    { TokenType::lessEqual, "lessEqual" },
    { TokenType::more, "more" },
    { TokenType::moreEqual, "moreEqual" },
    { TokenType::comma, "comma" },
    { TokenType::semicolon, "semicolon" },
    { TokenType::colon, "colon" },
    { TokenType::dot, "dot" },
    { TokenType::exclamation, "exclamation" },
    { TokenType::doubleAmpersand, "doubleAmpersand" },
    { TokenType::ampersand, "ampersand" },
    { TokenType::bar, "bar" },
    { TokenType::doubleBar, "doubleBar" },
    { TokenType::parenStart, "parenStart" },
    { TokenType::parenEnd, "parenEnd" },
    { TokenType::curlyStart, "curlyStart" },
    { TokenType::curlyEnd, "curlyEnd" },
    { TokenType::squareStart, "squareStart" },
    { TokenType::squareEnd, "squareEnd" },
    { TokenType::quote, "quote" },
    { TokenType::doubleQuote, "doubleQuote" },
    { TokenType::backTick, "backTick" },

};

static std::string_view tokenTypeName(TokenType tt){
    return tokenTypeNameLookup[tt];
}

static bool isSymbol(const std::string &prefix){
    return symbolLookup.count(prefix);
}

static TokenType symbolToTokenType(std::string_view sym){
    return symbolLookup[sym];
}

static bool isKeyword(const std::string &word){
    return keywordLookup.count(word);
}

static TokenType keywordToTokenType(std::string_view word){
    return keywordLookup[word];
}