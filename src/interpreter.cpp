#include "../headers/utils.hpp"
#include "../headers/interpreter.hpp"
#include "../headers/lexer.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void repl(){
    std::cout << "P[ainfully] F[unctional] L[anguage] Early Development Build" << std::endl;
    std::cout << "Tip - Type\033[36m q\033[0m to quit" << std::endl;
    std::stringstream sstream;
    std::string line;
    Lexer lexer = Lexer(&sstream);
    while(true){
        std::cout << "\n\033[36m" << ">> ";
        std::getline(std::cin, line);
        std::cout << "\033[0m";
        if(line == "q" || line == "quit"){
            break;
        }
        sstream.clear();
        sstream << line;
        while(!line.empty() && line.back() == '\\'){
            std::cout << "\033[36m" << ">> ";
            std::getline(std::cin, line);
            std::cout << "\033[0m";
            sstream << line;
        };
        int lineNum = 0;
        try {
            std::vector<Token> tokens = lexer.getRemainingTokens();
            std::cout << "[ ";
            for(const Token &token : tokens){
                std::cout << tokenTypeName(token.type) << "(" << token.text << ") ";
            }
            std::cout << "]" << std::endl;
        } catch(LexerError err){
            std::cout << err.what() << std::endl;
        }

    }

}

void interpreter(const std::string &path){
    std::ifstream is(path);
    Lexer lexer = Lexer(&is);
    try {
        std::vector<Token> tokens = lexer.getRemainingTokens();
        std::cout << "[ ";
        for(const Token &token : tokens){
            std::cout << tokenTypeName(token.type) << "(" << token.text << ") ";
        }
        std::cout << "]" << std::endl;

    } catch(LexerError err){
        std::cout << err.what() << std::endl;
    }
}