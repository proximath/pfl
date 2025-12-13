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
    ExprNode* handleFnParamList();
    ExprNode* handleBlock();
    ExprNode* handleFn();
    ExprNode* handleTypeSyntax();
    ExprNode* handleIf();
    ExprNode* handleCallArgsList();
    ExprNode* handleArrayLiteral();
    ExprNode* handleArraySubscript();
    ExprNode* handleStringTemplate();
    ExprNode* handleFormatString();
    ExprNode* handleExpression(std::vector<TokenType>);
    ExprNode* handleStruct();
    ExprNode* handleEnum();
    ExprNode* handleMatch();
    ExprNode* tryTuplePattern(TokenType);
    ExprNode* tryTupleExpression(TokenType);
    ExprNode* tryAssignment();
    ExprNode* tryTypedIdentifier();
    void emitError(const std::string&);
    void popOperatorStack(std::vector<ExprNode*>&, ExprNode*&, ExprNode*&);
    void addCheckpoint();
    void restoreCheckpoint();
    void commitCheckpoint();
public:
    ExprNode* parse(std::vector<Token>);
};
