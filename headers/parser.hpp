#pragma once

#include "../headers/utils.hpp"
#include "../headers/token.hpp"
#include "ast.hpp"

#include <stack>

class Parser {
private:
    std::vector<Token> tokens;
    int tokenInd = 0;

    Token& getPrevToken();
    Token& getCurToken();
    Token& expectToken(TokenType);
    Token* discardToken(TokenType);
    Token* tryToken(TokenType);
    AstNode* handleFnParamList();
    AstNode* handleBlock();
    AstNode* handleFn();
    AstNode* handleIf();
    AstNode* handleCallArgsList();
    AstNode* handleArrayLiteral();
    AstNode* handleExpression(std::vector<TokenType>);
    AstNode* tryAssignment(std::vector<TokenType>);
    void emitError(const std::string&);
    void popOperatorStack(std::vector<AstNode*>&, AstNode*&, AstNode*&);
public:
    AbstractSyntaxTree parse(std::vector<Token>);
};

NodeType tokenToBinaryOperator(TokenType);
NodeType tokenToUnaryOperator(TokenType);
NodeType tokenToPrimary(TokenType);