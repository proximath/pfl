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
    Token& expectToken(TokenType, const std::string&);
    Token* discardToken(TokenType);
    Token* tryToken(TokenType);
    bool isCurToken(TokenType);
    FunctionParam* handleFnParam();
    void handleFnParamList(Function*);
    ExprNode* handleBlock();
    ExprNode* handleFn();
    TypeNode* handleTypeNode();
    ExprNode* handleIf();
    ExprNode* handleCallArgsList();
    ExprNode* handleArrayLiteral();
    ExprNode* handleArraySubscript();
    ExprNode* handleStringTemplate();
    ExprNode* handleFormatString();
    ExprNode* handleExpression(std::vector<TokenType>);
    StructMethod* handleStructMethod();
    StructAttribute* handleStructAttribute();
    ExprNode* handleStruct();
    ExprNode* handleEnum();
    ExprNode* handleMatch();
    TuplePatternBase* tryTuplePattern(TokenType);
    ExprNode* tryTupleExpression(TokenType);
    ExprNode* tryAssignment();
    ExprNode* tryTypedIdentifier();
    void handleGenericDecl(Structure*);
    void emitError(const std::string&);
    void popOperatorStack(std::vector<ExprNode*>&, ExprNode*&, ExprNode*&);
    void addCheckpoint();
    void restoreCheckpoint();
    void commitCheckpoint();
public:
    ExprNode* parse(std::vector<Token>);
};
