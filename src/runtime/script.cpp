#include "../include/interpreter.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"

void script(const std::string &path){
    std::ifstream is(path);
    if(!is.is_open()){
        std::cerr << "Could not open file `" << path << "`" << std::endl;
        return;
    }
    Lexer lexer = Lexer(&is);
    try {
        std::vector<Token> tokens = lexer.getTokens();
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
    } catch(ParserError err){
        std::cout << err.what() << std::endl;
    }
}