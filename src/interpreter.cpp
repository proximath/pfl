#include "../headers/utils.hpp"
#include "../headers/interpreter.hpp"
#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"

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
    Parser parser;
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
            AbstractSyntaxTree ast = parser.parse(tokens);
            ast.print(ast.root, 0);
        } catch(LexerError err){
            std::cerr << err.what() << std::endl;
            return;
        } catch(ParserError err){
            std::cerr << err.what() << std::endl;
            return;
        } catch(SystemError err){
            std::cerr << err.what() << std::endl;
            return;
        }
    }
}

void interpreter(const std::string &path){
    std::ifstream is(path);
    if(!is.is_open()){
        std::cerr << "Could not open file `" << path << "`" << std::endl;
        return;
    }
    Lexer lexer = Lexer(&is);
    try {
        std::vector<Token> tokens = lexer.getRemainingTokens();
        std::cout << "[ ";
        for(const Token &token : tokens){
            std::cout << tokenTypeName(token.type) << "(" << token.text << ") ";
        }
        std::cout << "]" << std::endl;
        Parser parser;
        AbstractSyntaxTree ast = parser.parse(tokens);
        ast.print(ast.root);
    } catch(LexerError err){
        std::cout << err.what() << std::endl;
    }
}