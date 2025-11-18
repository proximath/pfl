#pragma once

#include "token.hpp"

#include <vector>
#include <functional>
#include <optional>
#include <stdexcept>

enum class State {
    placeholder, // Don't use
    begin,
    word,
    space,
    number,
    symbol,
    comment,
    multiComment,
    string,
    end,
};

static constexpr std::array symbolList = {
    "+", "-", "**", "*", "//", "/", "==", "=", ">=", ">", "<=", "<",
    ",", ";", ".", ":",
    "!", "&&", "&", "||", "&",
    "[", "]", "(", ")", "{{", "}}" "{", "}",
    "\'", "\"", "`"
};

bool isSymbol(const std::string&);
TokenType stateToTokenType(State);
std::vector<Token> lexer(std::function<std::optional<std::string>()>);
