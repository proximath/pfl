#pragma once

#include "token.hpp"

#include <vector>
#include <functional>
#include <optional>
#include <stdexcept>
#include <fstream>

enum class State {
    placeholder, // Don't use
    normal,
    word,
    space,
    number,
    symbol,
    comment,
    multiComment,
    string,
    leadingSpace,
    newline,
    escape,
};

enum class ReadLineStatus {
    readNewLine,
    noNewLine,
    endReached,
};

class Lexer {
private:
    int lineNum = 0;
    int colNum = 0;
    int prefixColStart = 0;
    bool eof = false;
    std::istream *stream;
    std::string line;
    std::string prefix = "";
    State curState = State::normal;
    int hashtagCount = 0;
    int hashtagUninterrupted = false;
    bool numberIsFloat = false;

    ReadLineStatus readLineIfEndOfLine();
    char ignoreChar();
    char consumeChar();
    char readChar();
    Token createNewToken();
    TokenType getTokenType();
    void emitError(const std::string &);
    void emitError(const std::string &, int);
    void resetVariables();

public:
    Lexer(std::istream *stream);
    std::optional<Token> getNextToken();
    std::vector<Token> getRemainingTokens(); 
};

std::string_view stateName(State);
TokenType stateToTokenType(State, const std::string&);
