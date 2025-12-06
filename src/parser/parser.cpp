#include "../include/parser.hpp"
#include "parser-utils.hpp"
#include "../token/operator.hpp"
#include "../ast/operator.hpp"

#include <algorithm>

AstNode* Parser::handleArrayLiteral(){
	expectToken(TokenType::squareStart);
	AstNode *returned = new AstNode(NodeType::arrayLiteral, ArrayLiteral{});
	while(getPrevToken().type != TokenType::squareEnd){
		AstNode *elem = handleExpression({ TokenType::comma, TokenType::squareEnd });
		returned->as<ArrayLiteral>().elements.push_back(elem);
		if(getCurToken().type == TokenType::squareEnd){
			break;
		}
	}
	return returned;
}

void Parser::emitError(const std::string &msg){
	throw ParserError(msg);
}

Token& Parser::getCurToken(){
	if(tokenInd >= tokens.size()){
		emitError("Parser::getCurToken index out of bounds");
	}
	return tokens[tokenInd];
}

Token& Parser::getPrevToken(){
	if(tokenInd > tokens.size() || tokenInd <= 0){
		emitError("Parser::getPrevToken index out of bounds");
	}
	return tokens[tokenInd - 1];
}

Token* Parser::discardToken(TokenType type){
	if(tokenInd < tokens.size() && tokens[tokenInd].type == type){
		return &(tokens[tokenInd++]);
	}
	return nullptr;
}

Token& Parser::expectToken(TokenType type){
	if(getCurToken().type != type){
		emitError(std::string("Expected token " + getTokenTypeName(type)));
	}
	return tokens[tokenInd++];
}

Token& Parser::expectToken(TokenType type, const char *msg){
	if(getCurToken().type != type){
		emitError(msg);
	}
	return tokens[tokenInd++];
}

Token* Parser::tryToken(TokenType type){
	if(tokenInd < tokens.size() && tokens[tokenInd].type == type){
		return &(tokens[tokenInd]);
	}
	return nullptr;
}

void Parser::addCheckpoint(){
	checkpoint.push_back(tokenInd);
}

void Parser::restoreCheckpoint(){
	tokenInd = checkpoint.back();
	checkpoint.pop_back();
}

void Parser::commitCheckpoint(){
	checkpoint.pop_back();
}

AstNode* Parser::parse(std::vector<Token> tokenStream){
	tokens.insert(tokens.end(), tokenStream.begin(), tokenStream.end());
	AstNode *root = new AstNode(NodeType::block, Block{});
	while(tokenInd < tokens.size()){
		AstNode *exp = handleExpression({ TokenType::newline });	
		root->as<Block>().expressions.push_back(exp);
	}
	//std::cout << "Finished parsing" << std::endl;
	return root;
}
