#include "../headers/utils.hpp"
#include "../headers/lexer.hpp"

Lexer::Lexer(std::istream *stream)
  : stream(stream)
{
}

ReadLineStatus Lexer::readLineIfEndOfLine(){
    if(colNum >= line.length()){
        lineNum++;
        if(stream->eof()){
            return ReadLineStatus::endReached;
        }
        colNum = 0;
        std::getline(*stream, line);
        line += '\n';
        return ReadLineStatus::readNewLine;
    }
    return ReadLineStatus::noNewLine;
}

char Lexer::consumeChar(){
    if(colNum >= line.length()){
        throw SystemError("out of bounds in Lexer::consumeChar", __FILE_NAME__, __LINE__);
    }
    prefix += line[colNum];
    colNum++;
    return line[colNum];
}

char Lexer::ignoreChar(){
    if(colNum >= line.length()){
        throw SystemError("out of bounds in Lexer::ignoreChar", __FILE_NAME__, __LINE__);
    }
    colNum++;
    return line[colNum];
}

char Lexer::readChar(){
    if(colNum >= line.length()){
        throw SystemError("out of bounds in Lexer::readChar", __FILE_NAME__, __LINE__);
    }
    return line[colNum];
}

TokenType Lexer::getTokenType(){
    switch(curState){
    case State::normal:
        emitError("State::normal has no token type");
    case State::word:
        if(isKeyword(prefix)){
            return keywordToTokenType(prefix);
        } else {
            return TokenType::identifier;
        }
    case State::number:
        if(prefix.find(".") != -1){
            if(!isdigit(prefix.back())){
                throw LexerError("Ill formed float literal", __LINE__, prefixColStart);
            }
            return TokenType::floatLiteral;
        } else {
            return TokenType::intLiteral;
        }
    case State::symbol:
        return symbolToTokenType(prefix);
    case State::string:
        if(prefix.back() != '\"'){
            throw LexerError("Ill formed string litteral (missing end quote)", __LINE__, prefixColStart);
        }
        return TokenType::string;
    case State::escape:
        return escapeToTokenType(prefix);
    }
    throw SystemError("Lexer::getTokenType, not implemented state", __FILE_NAME__, __LINE__);
}

Token Lexer::createNewToken(){
    Token returned = Token{ lineNum, colNum, prefix, getTokenType() }; 
    prefix = "";
    prefixColStart = colNum;
    return returned;
}

void Lexer::resetVariables(){
    numberIsFloat = false;
    hashtagCount = 0;
    hashtagUninterrupted = false;
}

void Lexer::emitError(const std::string &msg){
    curState = State::normal;
    line.clear();
    prefix.clear();
    lineNum++;
    colNum = 0;
    resetVariables();
    throw LexerError(msg, lineNum, prefixColStart);
}

void Lexer::emitError(const std::string &msg, int col){
    curState = State::normal;
    line.clear();
    prefix.clear();
    lineNum++;
    colNum = 0;
    resetVariables();
    throw LexerError(msg, lineNum, col);
}

std::optional<Token> Lexer::getNextToken(){
    Token returned;
    int cnt = 0;
    while(true){
        if(cnt == 10) throw;
        cnt++;
        //std::cerr << colNum << " " << stateName(curState) << std::endl;
        ReadLineStatus readStatus = readLineIfEndOfLine();
        if(readStatus == ReadLineStatus::readNewLine){
            //std::cerr << "READED: " << line << std::endl;
        }
        if(readStatus == ReadLineStatus::endReached){
            return std::optional<Token>();
        }
        char c = line[colNum];
        switch(curState){
        case State::normal:
            if(isalpha(c)){
                curState = State::word;
            } else if(isdigit(c)){
                curState = State::number;
            } else if(c == '\"'){
                curState = State::string;
            } else if(c == '#'){
                hashtagCount = 1;
                hashtagUninterrupted = true;
                curState = State::comment;
            } else if(isSymbol(c)){
                curState = State::symbol;
            } else if(c == '\n'){
                curState = State::newline;
            } else if(isspace(c)){
                curState = State::space;
            } else if(c == '\\'){
                curState = State::escape;
            } else {
                throw SystemError("State::normal not implemented", __FILE_NAME__, __LINE__);
            }
            break;
        case State::word:
            while(isalnum(c) || c == '_'){
                c = consumeChar();
            }
            returned = createNewToken();
            curState = State::normal;
            return returned;
        case State::number:
            if(isdigit(c)){
                consumeChar();
            } else if(c == '.'){
                if(numberIsFloat){
                    emitError("Ill formed float literal (multiple decimal separator)", colNum);
                }
                consumeChar();
                numberIsFloat = true;
            } else if(isalpha(c)){
                emitError("Identifier may not start with a number");
            } else {
                returned = createNewToken();
                curState = State::normal;
                return returned;
            }
            break;
        case State::symbol:
            c = readChar();
            while(isSymbol(prefix + c)){
                c = consumeChar();
            }
            returned = createNewToken();
            curState = State::normal;
            return returned;
            break;
        case State::string:
            c = consumeChar();
            while(c != '\"'){
                if(c == '\n'){
                    emitError("Missing string delimeter", colNum);
                }
                if(c == '\\'){
                    throw SystemError("State::token inimplemented feature", __FILE_NAME__, __LINE__);
                    createNewToken();
                }
                c = consumeChar();
            }
            consumeChar();
            returned = createNewToken();
            curState = State::normal;
            return returned;
            break;
        case State::space:
            while(isspace(c)){
                c = ignoreChar();
            }
            curState = State::normal;
            break;
        case State::comment:
            while(true){
                c = ignoreChar();
                //std::cerr << colNum << " " << hashtagCount << " " << hashtagUninterrupted  << std::endl;
                if(c == '#'){
                    hashtagCount++;
                } else if(c == '\n'){
                    hashtagCount = 0;
                    hashtagUninterrupted = false;
                    curState = State::newline;
                    break;
                } else {
                    hashtagUninterrupted = false;
                    hashtagCount = 0;
                } 
                if(hashtagCount == 3 && hashtagUninterrupted){
                    hashtagCount = 0;
                    hashtagUninterrupted = false;
                    curState = State::multiComment;
                    break;
                }
            }
            break;
        case State::multiComment:
            while(true){
                c = ignoreChar();
                if(c == '#'){
                    hashtagUninterrupted = true;
                    hashtagCount++;
                } else if(c == '\n'){
                    ignoreChar();
                    hashtagCount = 0;
                    hashtagUninterrupted = false;
                    ReadLineStatus readStatus = readLineIfEndOfLine();
                    if(readStatus == ReadLineStatus::endReached){
                        throw LexerError("Multi-line comment not terminated");
                    }
                    break;
                } else {
                    hashtagUninterrupted = false;
                    hashtagCount = 0;
                }
                if(hashtagCount == 3 && hashtagUninterrupted){
                    ignoreChar();
                    hashtagCount = 0;
                    hashtagUninterrupted = false;
                    curState = State::normal;
                    break;
                }

            }
            break;
        case State::newline:
            ignoreChar();
            curState = State::normal;
            break;
        case State::escape:
            c = ignoreChar();
            std::cerr << int(c) << std::endl;
            if(c == '\n'){
                curState = State::newline;
            } else {
                throw SystemError("State::escape unimplemented feature", __FILE__, __LINE__);
            }
            break;
        default:
            std::cerr << stateName(curState) << std::endl;
            throw SystemError("Unknown state in Lexer::getNextToken", __FILE__, __LINE__);
        } 
    }
}

std::vector<Token> Lexer::getRemainingTokens(){
    std::vector<Token> returned;
    for(std::optional<Token> tok = getNextToken(); tok.has_value();){
        returned.push_back(tok.value());
        tok = getNextToken();
    }
    return returned;
}

std::string_view stateName(State state){
    switch(state){
    case State::normal:
        return "normal";
    case State::word:
        return "word";
    case State::number:
        return "number";
    case State::space:
        return "space";
    case State::comment:
        return "comment";
    case State::multiComment:
        return "multiComment";
    case State::symbol:
        return "symbol";
    case State::escape:
        return "escape";
    case State::string:
        return "string";
    case State::newline:
        return "newline";
    default:
        throw SystemError("function `stateName` is unimplemented!", __FILE_NAME__, __LINE__);
    }
}