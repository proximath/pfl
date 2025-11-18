#include "../headers/utils.hpp"
#include "../headers/interpreter.hpp"
#include "../headers/lexer.hpp"

#include <iostream>
#include <fstream>

void repl(){
    std::vector<std::string> src;
    std::string line;
    while(true){
        std::cout << ">> ";
        std::getline(std::cin, line);
        if(line == "q" || line == "quit"){
            break;
        }
        src.clear();
        while(!line.empty() && line.back() == '\\'){
            src.push_back(line);
            std::cout << ">> ";
            std::getline(std::cin, line);
        };
        src.push_back(line);
        int lineNum = 0;
        try {
            std::vector<Token> lexemes = lexer([&src, &lineNum](){
                if(lineNum == src.size()){
                    return std::optional<std::string>();
                }
                return std::optional<std::string>(src[lineNum++]);
            });
            std::cout << "[ ";
            for(const Token &lexeme : lexemes){
                std::cout << lexeme.text << ", ";
            }
            std::cout << "]" << std::endl;
        } catch(LexerError err){
            std::cout << err.what() << std::endl;
        }

    }

}

void interpreter(const std::string &path){
    std::ifstream src(path);
    std::string line;
    try {
        std::vector<Token> lexemes = lexer([&src, &line](){
            if(src.eof()){
                return std::optional<std::string>();
            }
            std::getline(src, line);
            return std::optional<std::string>(line);
        });
        std::cout << "[ ";
        for(const Token &lexeme : lexemes){
            std::cout << lexeme.text << ", ";
        }
        std::cout << "]" << std::endl;

    } catch(LexerError err){
        std::cout << err.what() << std::endl;
    }
}