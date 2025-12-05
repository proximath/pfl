#include "../include/utils.hpp"
#include "../include/lexer.hpp"

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
    case State::newline:
        return TokenType::newline;
    case State::leadingSpace:
        throw SystemError("Dont use this case", __FILE_NAME__, __LINE__);
        if(indentSpace % indentSpace){
            emitError("Inconsistent indent spacing");
        }
        int tmp = indentLevel;
        if(indentLevel > prevIndentLevel){
            while(tmp){

            }
        }
        //std::cout << indentLevel << " " << prevIndentLevel << " " << indentSpace << std::endl;
        //prefix = std::move(std::to_string((indentLevel - prevIndentLevel) / indentSpace));
        return TokenType::indent;
    }
    throw SystemError("Lexer::getTokenType, not implemented state", __FILE_NAME__, __LINE__);
}

Token Lexer::createNewToken(){
    TokenType type = getTokenType();
    Token returned = Token{ lineNum, colNum, prefix, type }; 
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

std::vector<Token> Lexer::getTokens(){
    std::vector<Token> tokens;
    while(true){
        //std::cerr << colNum << " " << stateName(curState) << std::endl;
        ReadLineStatus readStatus = readLineIfEndOfLine();
        if(readStatus == ReadLineStatus::readNewLine){
            // std::cerr << "READED: " << line << std::endl;
        }
        if(readStatus == ReadLineStatus::endReached){
            return tokens;
        }
        char c = line[colNum];
        switch(curState){
        case State::normal:
            if(isalpha(c) || c == '_'){
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
                if(colNum == 0){
                    curState = State::leadingSpace;
                } else {
                    curState = State::space;
                }
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
            tokens.push_back(createNewToken());
            curState = State::normal;
            break;
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
                tokens.push_back(createNewToken());
                curState = State::normal;
            }
            break;
        case State::symbol:
            c = readChar();
            while(isSymbol(prefix + c)){
                c = consumeChar();
            }
            tokens.push_back(createNewToken());
            curState = State::normal;
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
            tokens.push_back(createNewToken());
            curState = State::normal;
            break;
        case State::space:
            while(isspace(c) && c != '\n'){
                c = ignoreChar();
            }
            curState = State::normal;
            break;
        case State::leadingSpace:
        {
            indentLevel = 0;
            while(isspace(c) && c != '\n'){
                c = ignoreChar();
                indentLevel++;
            }
            if(c == '\n'){
                ignoreChar();
                break;
            }
            if(indentSpace == -1 && indentLevel != 0){
                indentSpace = indentLevel;
            }
            if((indentLevel - prevIndentLevel) % indentSpace){
                emitError("Inconsistent spacing");
            }
            int cnt = abs((indentLevel - prevIndentLevel) / indentSpace);
            TokenType indentType;
            if(indentLevel > prevIndentLevel){
                indentType = TokenType::indent;
            } else if(indentLevel < prevIndentLevel){
                indentType = TokenType::dedent;
            }
            while(cnt--){
                tokens.push_back(Token{lineNum, colNum, "", indentType });
            }
            prevIndentLevel = indentLevel;
            curState = State::normal;
            break;
        }
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
            tokens.push_back(createNewToken());
            curState = State::leadingSpace;
            break;
        case State::escape:
            c = ignoreChar();
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

std::unordered_map<State, const char*> stateNameLookup = {
    { State::normal, "normal" },
    { State::word, "word" },
    { State::number, "number" },
    { State::space, "space" },
    { State::comment, "comment" },
    { State::multiComment, "multiComment" },
    { State::symbol, "symbol" },
    { State::escape, "escape" },
    { State::string, "string" },
    { State::newline, "newline" },
};

std::string_view stateName(State state){
    if(!stateNameLookup.count(state)){
        throw SystemError("function `stateName` this state is unimplemented!", __FILE_NAME__, __LINE__);
    }
    return stateNameLookup[state];
}