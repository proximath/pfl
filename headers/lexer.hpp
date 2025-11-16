#pragma once

#include <string>
#include <array>
#include <fstream>
#include <vector>

enum class TokenType {
    intLiteral,
    floatLiteral,
    stringLiteral,
    rawStringLiteral,
    identifier,
    symbol
};

struct Token {
    int line_num;
    int column_num;
    std::string text;
    TokenType type;
};

std::array symbolList = {
    '+', '-', '*', '/', '=', '%',
    '(', ')', '[', ']', '{', '}',
    '.', ',',
    '\'', '\"',
    '#',
};

bool isSymbol(char tok){
    for(char s : symbolList){
        if(s == tok){
            return true;
        }
    }
    return false;
}

std::vector<Token> lexer(std::ifstream);