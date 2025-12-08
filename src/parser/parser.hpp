#pragma once

#include "../include/utils.hpp"
#include "../token/token.hpp"
#include "../ast/astnode.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    std::vector<int> checkpoint;
    int tokenInd = 0;

    Token& getPrevToken();
    Token& getCurToken();
    Token& expectToken(TokenType);
    Token& expectToken(TokenType, const char *);
    Token* discardToken(TokenType);
    Token* tryToken(TokenType);
    AstNode* handleFnParamList();
    AstNode* handleBlock();
    AstNode* handleFn();
    AstNode* handleIf();
    AstNode* handleCallArgsList();
    AstNode* handleArrayLiteral();
    AstNode* handleArraySubscript();
    AstNode* handleStringTemplate();
    AstNode* handleFormatString();
    AstNode* handleExpression(std::vector<TokenType>);
    AstNode* tryTuplePattern(TokenType);
    AstNode* tryTupleExpression(TokenType);
    AstNode* tryAssignment();
    AstNode* tryTypedIdentifier();
    void emitError(const std::string&);
    void popOperatorStack(std::vector<AstNode*>&, AstNode*&, AstNode*&);
public:
    AstNode* parse(std::vector<Token>);
    void addCheckpoint();
    void restoreCheckpoint();
    void commitCheckpoint();
};
