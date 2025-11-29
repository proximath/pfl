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
        } else if(node->type == NodeType::identifier){
            std::cout << getNodeTypeName(node->type) << " | " << node->as<Identifier>().name << std::endl; 
        } else if(node->type == NodeType::fnParamList){
            std::cout << getNodeTypeName(node->type) << std::endl;
            for(AstNode *param : node->as<FnParamList>().params){
                print(param, level + 1);
            }
        } else if(node->type == NodeType::function){
            std::cout << getNodeTypeName(node->type) << std::endl;
            print(node->as<Function>().name, level + 1);
            print(node->as<Function>().paramList, level + 1);
            print(node->as<Function>().block, level + 1);
        } else if(node->type == NodeType::block){
            std::cout << getNodeTypeName(node->type) << std::endl;
            for(AstNode *expr : node->as<Block>().expressions){
                print(expr, level + 1);
            }
        } else {
            throw SystemError("AbstractSyntaxTree::print unimplemented", __FILE_NAME__, __LINE__);
        }
    }
};

