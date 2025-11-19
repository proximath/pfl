#include "../headers/utils.hpp"
#include "../headers/lexer.hpp"

Lexer::Lexer(std::istream *stream)
  : stream(stream)
{
}

void Lexer::resetState(){
    prefix = "";
    curState = State::space;
    hashtagCount = 0;
    numberIsFloat = false;
    insideString = false;
}

void pretest(){

}

void posttest(){

}

std::pair<bool, State> Lexer::stateMachine(char c){
    std::cerr << stateName(curState) << ": " << c << std::endl;
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
            return { false, State::number };
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
        } else if(prefix == "." && isdigit(c)){
            numberIsFloat = true;
            return { false, State::number };
        } else if(prefix == "-" && isdigit(c)){
            return { false, State::number };
        }
        break;
    case State::string:
        if(c == '\"'){
            return { true, State::space };
        } else {
            return { false, State::string };
        }
        break;
    case State::comment:
        if(c == '#'){
            hashtagCount++;
        }
        if(hashtagCount == 3){
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
        if(hashtagCount == 3){
            hashtagCount = 0;
            return { false, State::space };
        }
    case State::escape:
        if(prefix == "\\" || isEscape(prefix + c)){
            return { false, State::escape };
        }
    }
    if(isspace(c) && curState != State::comment && curState != State::multiComment && curState != State::string){
        return { true, State::space };
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
    if(isspace(c)){
        return { true, State::space };
    }
    if(c == '\"'){
        insideString = true;
        return { true, State::string };
    }   
    if(c == '\\'){
        return { true, State::escape };
    }
    if(c == '\n'){
        return { true, State::space };
    }
    if(isSymbol(std::string(1, c))){
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
                colNum++;
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
        return symbolToTokenType(text);
    case State::string:
        if(text.back() != '\"'){
            std::cerr << text << std::endl;
            throw LexerError("Ill formed string litteral (missing end quote)");
        }
        return TokenType::string;
    case State::escape:
        return escapeToTokenType(text);
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
    case State::symbol:
        return "symbol";
    case State::escape:
        return "escape";
    case State::string:
        return "string";
    default:
        throw SystemError("function `stateName` is unimplemented!", __LINE__, 5);
    }
}