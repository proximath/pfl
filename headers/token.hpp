#include "utils.hpp"

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
};

struct Token {
    int line_num;
    int column_num;
    std::string text;
    TokenType type;
};

std::string_view tokenTypeName(TokenType tt){
    throw SystemError("tokenTypeName is unimplemented!", __LINE__, 5);
}