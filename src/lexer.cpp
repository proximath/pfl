#include "../headers/utils.hpp"
#include "../headers/lexer.hpp"

Lexer::Lexer(std::istream *stream)
  : stream(stream)
{
}

void Lexer::resetState(){
    prefix = "";
    curState = State::space;
    colNum = 0;
    hashtagCount = 0;
    numberIsFloat = false;
}

std::pair<bool, State> Lexer::stateMachine(char c){
    if(isspace(c) && curState != State::comment && curState != State::multiComment){
        return { true, State::space };
    }
    switch(curState){
    case State::word:
        if(isalpha(c) || isdigit(c)){
            return { false, State::word };
        }
        break;
    case State::number:
        if(isdigit(c)){
            return { false, State::number };
        } else if(c == '.'){
            if(numberIsFloat){
                resetState();
                throw LexerError("Ill formed number", lineNum, tokenColStart);
            }
            numberIsFloat = true;
        } else if(isalpha(c)){
            resetState();
            throw LexerError("Identifier may not start with a number", lineNum, tokenColStart);
        } else {
            numberIsFloat = false;
        }
        break;
    case State::symbol:
        if(isSymbol(prefix + c)){
            return { false, State::symbol };
        }
        break;
    case State::comment:
        if(c == '#'){
            hashtagCount++;
        } else if(c == '\n'){
            hashtagCount = 0;
            return { true, State::space };
        }
        if(hashtagCount == 3 && prefix.length() == 3){
            hashtagCount = 0;
            return { false, State::multiComment };
        } else {
            return { false, State::comment };
        }
        break;
    case State::multiComment:
        if(c == '#'){
            hashtagCount++;
        } else {
            hashtagCount = 0;
        }
        if(hashtagCount == 3 && prefix.length() == 3){
            hashtagCount = 0;
            return { false, State::space };
        }
    }
    if(c == '#' && curState != State::comment && curState != State::multiComment){
        hashtagCount = 1;
        return { true, State::comment };
    }
    if(isalpha(c)){
        return { true, State::word };
    }
    if(isdigit(c)){
        return { true, State::number };
    }
    if(isSymbol(prefix + c)){
        return { true, State::symbol };
    }
    throw SystemError("Unreachable point in function Lexer::stateMachine", __LINE__, 5);    
}

std::optional<Token> Lexer::getNextToken(){
    while(true){
        if(colNum == line.length()){
            if(stream->eof()){
                return {};
            }
            std::getline(*stream, line);
            line += '\n';
            lineNum++;
            colNum = 0;
        }
        for(; colNum < line.length(); colNum++){
            auto[push, newState] = stateMachine(line[colNum]);
            if(push && curState != State::space && curState != State::comment){
                Token ret = Token{
                    lineNum,
                    tokenColStart,
                    prefix,
                    stateToTokenType(curState, prefix)
                };
                curState = newState;
                if(newState == State::space){
                    prefix = "";
                } else {
                    prefix = line[colNum];
                }
                return ret;
            }
            curState = newState;
            if(curState != State::space && curState != State::comment){
                prefix += line[colNum];
            }
        }
        
    }
}

std::vector<Token> Lexer::getRemainingTokens(){
    std::vector<Token> res;
    for(std::optional<Token> tok = getNextToken(); tok.has_value();){
        res.push_back(tok.value());
        tok = getNextToken();
    }
    return res;
}

TokenType stateToTokenType(State state, const std::string &text){
    switch(state){
    case State::word:
        if(isKeyword(text)){
            return TokenType::keyword;
        } else {
            return TokenType::identifier;
        }
        return TokenType::identifier;
    case State::number:
        if(text.find(".") != -1){
            if(!isdigit(text.back())){
                throw LexerError("Ill formed float literal");
            }
            return TokenType::floatLiteral;
        } else {
            return TokenType::intLiteral;
        }
    case State::symbol:

        throw SystemError("stateToTokenType symbol not implemented", __LINE__, 9);
    }

    throw SystemError("Function stateToTokenType is not implemented", __LINE__, 5);
}

std::string_view stateName(State state){
    switch(state){
    case State::word:
        return "word";
    case State::number:
        return "number";
    case State::space:
        return "space";
    case State::comment:
        return "comment";
    default:
        throw SystemError("function `stateName` is unimplemented!", __LINE__, 5);
    }
}