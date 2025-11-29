#pragma once

#include "../headers/utils.hpp"
#include "../headers/token.hpp"
#include "ast.hpp"

#include <stack>

class Parser {
private:
    std::vector<Token> tokens;
    int tokenInd = 0;

    Token& getCurToken();
    Token& expectToken(TokenType);
    Token* discardToken(TokenType);
    AstNode* handleFnParamList();
    AstNode* handleBlock();
    AstNode* handleFn();
    AstNode* handleExpression(TokenType);
    void emitError(const std::string&);
public:
    AbstractSyntaxTree parse(std::vector<Token>);
};

NodeType tokenToBinaryOperator(TokenType);
NodeType tokenToUnaryOperator(TokenType);
NodeType tokenToPrimary(TokenType);