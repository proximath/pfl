#pragma once

#include "utils.hpp"
#include <unordered_map>
#include <unordered_set>

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
    notEqual,
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
    elseKeyword,
    elifKeyword,
    forKeyword,
    doKeyword,
    fnKeyword,
    trueKeyword,
    falseKeyword,
    typeKeyword,
    newline,
    tab,
    carriageReturn,
    escapeQuote,
    escapeDoubleQuote,
    escapeFormatStart,
    escapeFormatEnd,
    indent,
    dedent,
    eof,
};

struct Token {
    int line_num;
    int column_num;
    std::string text;
    TokenType type;
};

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
};

static std::unordered_map<TokenType, std::string> tokenTypeNameLookup = {
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
    { TokenType::notEqual, "notEqual" },
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
    { TokenType::ifKeyword, "ifKeyword" },
    { TokenType::forKeyword, "forKeyword" },
    { TokenType::doKeyword, "doKeyword" },
    { TokenType::fnKeyword, "fnKeyword" },
    { TokenType::newline, "newline" },
    { TokenType::tab, "tab" },
    { TokenType::carriageReturn, "carriageReturn" },
    { TokenType::escapeQuote, "escapeQuote" },
    { TokenType::escapeDoubleQuote, "escapeDoubleQuote" },
    { TokenType::escapeFormatStart, "escapeFormatStart" },
    { TokenType::escapeFormatEnd, "escapeFormatEnd" },
    { TokenType::indent, "indent" },
    { TokenType::dedent, "dedent" },
    { TokenType::eof, "eof" },
};

static std::unordered_map<std::string, TokenType> escapeCharsLookup = {
    { "\\n", TokenType::newline },
    { "\\t", TokenType::tab },
    { "\\r", TokenType::carriageReturn },
    { "\\\'", TokenType::escapeQuote },
    { "\\\"", TokenType::escapeDoubleQuote },
    { "\\{", TokenType::escapeFormatStart },
    { "\\}", TokenType::escapeFormatEnd },
};

static std::unordered_set<TokenType> operatorLookup = {
    TokenType::plus,
    TokenType::minus,
    TokenType::asterisk,
    TokenType::slash,
    TokenType::exponent,
    TokenType::root,
    TokenType::equal,
    TokenType::doubleEqual,
    TokenType::notEqual,
    TokenType::less,
    TokenType::lessEqual,
    TokenType::more,
    TokenType::moreEqual,
    TokenType::dot,
};

static const std::string& tokenTypeName(TokenType tt){
    return tokenTypeNameLookup[tt];
}

static bool isSymbol(const std::string &prefix){
    return symbolLookup.count(prefix);
}

static bool isSymbol(char c){
    return symbolLookup.count(std::string(1, c));
}

static TokenType symbolToTokenType(const std::string &sym){
    return symbolLookup[sym];
}

static bool isKeyword(const std::string &word){
    return keywordLookup.count(word);
}

static TokenType keywordToTokenType(const std::string &word){
    return keywordLookup[word];
}

static TokenType escapeToTokenType(const std::string &esc){
    return escapeCharsLookup[esc];
}

static bool isEscape(const std::string &esc){
    return escapeCharsLookup.count(esc);
}

static bool isPrimary(const TokenType type){
    return 
    type == TokenType::intLiteral || 
    type == TokenType::floatLiteral || 
    type == TokenType::string ||
    type == TokenType::identifier;
}

static bool isPrimary(const Token &token){
    return isPrimary(token.type);
}

static bool isOperator(const TokenType type){
    return operatorLookup.count(type);
}

static bool isOperator(const Token &token){
    return isOperator(token.type);
}

static bool isOpeningBrace(const TokenType type){
    return
    type == TokenType::parenStart ||
    type == TokenType::curlyStart ||
    type == TokenType::squareStart;
}

static bool isOpeningBrace(const Token &token){
    return isOpeningBrace(token.type);
}

static TokenType getMatchingBrace(const TokenType braceType){
    switch(braceType){
    case TokenType::parenStart:
        return TokenType::parenEnd;
    case TokenType::curlyStart:
        return TokenType::curlyEnd;
    case TokenType::squareStart:
        return TokenType::squareEnd;
    default:
        throw SystemError("At getMatchingBrace not a brace", __FILE_NAME__, __LINE__);
    }
}

static bool isPrefixOp(const TokenType type){
    return false;
}

static bool isPostfixOp(const TokenType type){
    return false;
}