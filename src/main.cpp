#include "runtime/interpreter.hpp"

#include <cstring>
#include <iostream>

int main(int argc, char *argv[]){
    bool hasSrcFile = false;
    bool quoteFilePath = false;
    bool forceRepl = false;
    std::string filePath = "";
    for(int i = 1; i < argc; i++){
        if(quoteFilePath){
            if(argv[i][std::strlen(argv[i]) - 1] == '\"'){
                quoteFilePath = false;
                argv[i][std::strlen(argv[i]) - 1] = '\0';
            } 
            filePath += argv[i];
        } else if(argv[i][0] == '-'){
            std::string arg = argv[i] + 1;
            if(arg == "r" || arg == "repl"){
                forceRepl = true;
            }
        } else if(!hasSrcFile){
            hasSrcFile = true;
            if(argv[i][0] == '\"'){
                quoteFilePath = true;
                filePath += argv[i]++;
            } 
            filePath += argv[i];
        }
    }
    if(!hasSrcFile || forceRepl){
        repl();
    } else {
        script(filePath);
    }
}