#pragma once

#include "../headers/utils.hpp"
#include "../headers/token.hpp"
#include "ast.hpp"

#include <stack>

struct Parentheses {
    int lineStart, colStart;
    int depth;
    char delimeter;
};

class Parser {
private:
    std::vector<Token> tokens;
    int spaceIndent = -1;
    int tabIndent = -1;
    std::stack<Parentheses> parenStack;
    int tokenInd = 0;

    AstNode* handleExpression(TokenType);
    void emitError(const std::string&);
public:
    AbstractSyntaxTree parse(std::vector<Token>);
};

NodeType tokenToBinaryOperator(TokenType);
NodeType tokenToUnaryOperator(TokenType);