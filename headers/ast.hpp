#pragma once

#include <variant>

struct Expression {

};

struct SumNode {
    Expression *left;
    Expression *right;
};

struct IfNode {
    Expression *condition;
    Expression *body;
};

struct AstNode {
    std::variant<Expression, IfNode> type;
};