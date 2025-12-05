#pragma once

#include "token.hpp"

static std::unordered_map<std::string, TokenType> escapeCharsLookup = {
    { "\\n", TokenType::newline },
    { "\\t", TokenType::tab },
    { "\\r", TokenType::carriageReturn },
    { "\\\'", TokenType::escapeQuote },
    { "\\\"", TokenType::escapeDoubleQuote },
    { "\\{", TokenType::escapeFormatStart },
    { "\\}", TokenType::escapeFormatEnd },
};

static TokenType escapeToTokenType(const std::string &esc){
    return escapeCharsLookup[esc];
}

static bool isEscape(const std::string &esc){
    return escapeCharsLookup.count(esc);
}