#pragma once

#include "../token/token.hpp"

#include <vector>
#include <functional>
#include <optional>
#include <stdexcept>
#include <fstream>

enum class State {
    normal,
    word,
    space,
    number,
    symbol,
    comment,
    multiComment,
    string,
    formatString,
    leadingSpace,
    newline,
    escape,
};

static std::unordered_map<State, const char*> stateNameLookup = {
    { State::normal, "normal" },
    { State::word, "word" },
    { State::number, "number" },
    { State::space, "space" },
    { State::comment, "comment" },
    { State::multiComment, "multiComment" },
    { State::symbol, "symbol" },
    { State::escape, "escape" },
    { State::string, "string" },
    { State::formatString, "formatString" },
    { State::newline, "newline" },
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
    int indentLevel = 0;
    int prevIndentLevel = 0;
    int indentSpace = -1;
    int stringTemplateLevel = 0;
    int formatStringLevel = 0;
    bool eof = false;
    std::istream *stream;
    std::string line;
    std::string prefix = "";
    State curState = State::leadingSpace;
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
    std::vector<Token> getTokens(); 
};

