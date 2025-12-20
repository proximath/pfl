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
    ExprNode* handleFn(std::vector<TokenType>);
    TypeNode* handleTypeNode();
    ExprNode* handleIf(std::vector<TokenType>);
    ExprNode* handleCallArgsList();
    ExprNode* tryKeywordCallArgsList();
    ExprNode* handleArrayLiteral();
    ExprNode* handleArraySubscript();
    ExprNode* handleStringTemplate();
    ExprNode* handleFormatString();
    ExprNode* handleExpression(std::vector<TokenType>, bool = true);
    StructMethod* handleStructMethod();
    StructAttribute* handleStructAttribute();
    ExprNode* handleStruct();
    EnumVariant* handleEnumVariant();
    ExprNode* handleEnum();
    ExprNode* handleMatch();
    TuplePatternBase* tryTuplePattern(TokenType);
    ExprNode* tryTupleExpression(TokenType);
    ExprNode* tryAssignment();
    void handleGenericDecl(std::vector<std::string>&);
    void emitError(const std::string&);
    void popOperatorStack(std::vector<ExprNode*>&, ExprNode*&, ExprNode*&);
    void addCheckpoint();
    void restoreCheckpoint();
    void commitCheckpoint();
public:
    ExprNode* parse(std::vector<Token>);
};
