#pragma once

#include "../headers/utils.hpp"
#include "../headers/token.hpp"
#include "../headers/node.hpp"

#include <iostream>

struct AbstractSyntaxTree {
    AstNode *root;
    AbstractSyntaxTree(AstNode *root)
      : root(root)
    {
    }
    void print(AstNode *node, int level = 0){
        for(int i = 0; i < 4 * level; i++){
            std::cout << " ";
        }
        std::cout << getNodeTypeName(node->type) << " | ";
    }
};

