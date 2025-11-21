#include "../headers/utils.hpp"
#include "../headers/interpreter.hpp"
#include "../headers/lexer.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

enum class ReplReadLineStatus {
    success,
    quit,
};

ReplReadLineStatus replReadLine(std::stringstream &stream, std::string &line){
    stream.clear();
    std::cout << "\n\033[36m" << ">> ";
    std::getline(std::cin, line);
    std::cout << "\033[0m";
    if(line == "q" || line == "quit"){
        return ReplReadLineStatus::quit;
    }
    while(line.empty()){
        std::cout << "\033[36m" << ">> ";
        std::getline(std::cin, line);
        std::cout << "\033[0m";
    }
    while(line.back() == '\\'){
        line.pop_back();
        stream << line << '\n';
        std::cout << "\033[36m" << ">> ";
        std::getline(std::cin, line);
        std::cout << "\033[0m";

    }
    stream << line;
    return ReplReadLineStatus::success;
}

void repl(){
    std::cout << "P[ainfully] F[unctional] L[anguage] Early Development Build" << std::endl;
    std::cout << "Tip - Type\033[36m q\033[0m to quit" << std::endl;
    std::stringstream sstream;
    std::string line;
    Lexer lexer = Lexer(&sstream);
    while(true){
        int lineNum = 0;
        if(replReadLine(sstream, line) == ReplReadLineStatus::quit){
            break;
        }
        try {
            std::vector<Token> tokens = lexer.getRemainingTokens();
            std::cout << "[ ";
            for(const Token &token : tokens){
                std::cout << tokenTypeName(token.type) << "(" << token.text << ") ";
            }
            std::cout << "]" << std::endl;
        } catch(LexerError err){
            std::cerr << err.what() << std::endl;
        } catch(SystemError err){
            std::cerr << err.what() << std::endl;
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