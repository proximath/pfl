#include "../headers/utils.hpp"
#include "../headers/lexer.hpp"

bool isSymbol(const std::string &prefix){
    for(const char *symbol : symbolList){
        if(prefix == symbol){
            return true;
        }
    }
    return false;
}

std::pair<bool, State> stateSpace(char next){
    if(std::isalpha(next)){
        return { true, State::word };
    } else if(std::isdigit(next)){
        return { true, State::number };
    } else if(std::isspace(next)){
        return { false, State::space };
    } else if(next == '#'){
        return { true, State::comment };
    } else {
        throw LexerError("LEXER ERROR: Unsupported symbol");
    }
}

std::pair<bool, State> stateWord(char next){
    if(std::isalpha(next)){
        return { false, State::word };
    } else if(std::isdigit(next)){
        return { false, State::word };
    } else if(std::isspace(next)){
        return { true, State::space };
    } else if(next == '#'){
        return { true, State::comment };
    } else {
        throw LexerError("ERROR");
    }
}

std::pair<bool, State> stateNumber(char next){
    if(std::isdigit(next)){
        return { false, State::number };
    } else if(std::isspace(next)){
        return { true, State::space };
    } else if(std::isalpha(next)){
        throw "ERR";
    } else if(next == '#'){
        return { true, State::comment };
    } else {
        throw LexerError("ERRowr");
    }
}

std::pair<bool, State> stateComment(char next){
    if(next == '\n'){
        return { true, State::space };
    } else {
        return { false, State::comment }; 
    }
}

std::pair<bool, State> stateBegin(char next){
    if(std::isalpha(next)){
        return { true, State::word };
    } else if(std::isdigit(next)){
        return { true, State::number };
    } else if(std::isspace(next)){
        return { true, State::space };
    } else if(next == '#'){
        return { true, State::comment };
    } else {
        throw LexerError("LEXER ERROR: Unsupported symbol");
    }
}

std::pair<bool, State> stateMachine(State state, char next){
    switch (state){
        case State::space:
            return stateSpace(next);
        case State::word:
            return stateWord(next);
        case State::number:
            return stateNumber(next);
        case State::comment:
            return stateComment(next);
        case State::begin:
            return stateBegin(next);
        default:
            throw LexerError("LEXER ERROR: Invalid State");    
    }
}

std::vector<Token> lexer(std::function<std::optional<std::string>()> getLine){
    int lineNum = 1;
    std::string prefix;
    std::vector<Token> res;
    std::string line;
    State curState; 
    while(true){
        curState = State::begin;
        int tokenColBegin = 0;
        prefix = "";
        std::optional<std::string> readed = getLine();
        if(!readed.has_value()){
            return res;
        }
        line = readed.value();
        std::cout << line << std::endl;
        for(int i = 0; i < line.length(); i++){
            auto[push, newState] = stateMachine(curState, line[i]);
            if(push && curState != State::begin){
                res.push_back({ lineNum, tokenColBegin, prefix, TokenType::placeholder });
                prefix = "";
                tokenColBegin = i + 1;
            }
            curState = newState;
            prefix += line[i];
        }
        if(curState != State::begin){
            res.push_back({ lineNum, tokenColBegin, prefix, TokenType::placeholder });
        }
        lineNum++;
    }
}