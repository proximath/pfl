#include "../include/lexer.hpp"

static std::string_view stateName(State state){
    if(!stateNameLookup.count(state)){
        throw SystemError("function `stateName` this state is unimplemented!", __FILE_NAME__, __LINE__);
    }
    return stateNameLookup[state];
}