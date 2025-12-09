#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <array>

class LexerError : public std::runtime_error {
public:
    LexerError(const std::string &msg, int lineNum, int colNum)
      : std::runtime_error("LEXER ERROR (" + std::to_string(lineNum) + ", " + std::to_string(colNum) + "): " + msg)
    {
    }
    LexerError(const std::string &msg)
      : std::runtime_error("LEXER ERROR: " + msg)
    {
    }
};

class ParserError : public std::runtime_error {
public:
    ParserError(const std::string &msg, int lineNum, int colNum)
      : std::runtime_error("PARSER ERROR (" + std::to_string(lineNum) + ", " + std::to_string(colNum) + "): " + msg)
    {
    }
    ParserError(const std::string &msg)
      : std::runtime_error("PARSER ERROR: " + msg)
    {
    }
};

class SystemError : public std::runtime_error {
public:
    SystemError(const std::string &msg, const char *fileName, int lineNum)
      : std::runtime_error("SYSTEM ERROR (" + std::to_string(lineNum) + ", " + fileName + "): " + msg)
    {
    }
    SystemError(const std::string &msg)
      : std::runtime_error("SYSTEM ERROR: " + msg)
    {
    }
};

class SemanticError : public std::runtime_error {
public:
    SemanticError(const std::string &msg, const char *fileName, int lineNum)
      : std::runtime_error("SEMANTIC ERROR (" + std::to_string(lineNum) + ", " + fileName + "): " + msg)
    {
    }
    SemanticError(const std::string &msg)
      : std::runtime_error("SEMANTIC ERROR: " + msg)
    {
    }
};



