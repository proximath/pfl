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
        if(node == nullptr){
            std::cout << "[NULL]" << std::endl;
            return;
        }
        for(int i = 0; i < 4 * level; i++){
            std::cout << " ";
        }
        if(isOperator(node->type)){
            if(isBinaryOperator(node->type)){
                std::cout << getNodeTypeName(node->type) << std::endl;
                // std::cout << node->as<BinaryOperation>().right << std::endl;
                print(node->as<BinaryOperation>().left, level + 1);
                print(node->as<BinaryOperation>().right, level + 1);
            } else {
                std::cout << getNodeTypeName(node->type) << std::endl;
                print(node->as<UnaryOperation>().expr, level + 1);
            }
        } else if(isPrimary(node->type)){
            std::cout << getNodeTypeName(node->type) << " | " << node->as<Primary>().text << std::endl;
        } else {
            throw SystemError("AbstractSyntaxTree::print unimplemented", __FILE_NAME__, __LINE__);
        }
    }
};

