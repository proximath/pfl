#pragma once

#include "token.hpp"

#include <vector>
#include <functional>
#include <optional>
#include <stdexcept>
#include <fstream>

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
    escape,
};

class Lexer {
private:
    int lineNum = 0;
    int colNum = 0;
    int tokenColStart = 0;
    std::istream *stream;
    std::string line;
    std::string prefix = "";
    State curState = State::space;
    int hashtagCount = 0;
    bool numberIsFloat = false;
    int insideString = false;
    std::pair<bool, State> stateMachine(char);
public:
    Lexer(std::istream *stream);
    void resetState();
    std::optional<Token> getNextToken();
    std::vector<Token> getRemainingTokens(); 
};

std::string_view stateName(State);
TokenType stateToTokenType(State, const std::string&);
