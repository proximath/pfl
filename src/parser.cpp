#include "../headers/parser.hpp"

NodeType tokenToBinaryOperator(TokenType type){
	switch(type){
	case TokenType::plus:
		return NodeType::addition;
	case TokenType::minus:
		return NodeType::subtraction;
	case TokenType::asterisk:
		return NodeType::multiplication;
	case TokenType::slash:
		return NodeType::division;
	case TokenType::exponent:
		return NodeType::exponentiation;
	default:
		throw SystemError("tokenToBinaryOperator unimplemented TokenType", __FILE_NAME__, __LINE__);
	}
}

NodeType tokenToUnaryOperation(TokenType type){
	switch(type){
	case TokenType::plus:
		return NodeType::plusSign;
	case TokenType::minus:
		return NodeType::minusSign;
	default:
		throw SystemError("tokenToUnaryOperator unimplemented TokenType", __FILE_NAME__, __LINE__);
	}
}

void Parser::emitError(const std::string &msg){
	throw ParserError(msg);
}

AstNode* Parser::handleExpression(TokenType delimeter){
	std::stack<AstNode*> stack;
	while(tokenInd < tokens.size()){
		Token &curToken = tokens[tokenInd];
		if(stack.empty() && isOperator(curToken)){
			stack.push(new AstNode{ 
				tokenToUnaryOperation(curToken.type), 
				UnaryOperation{
					new AstNode,
				}
			});
		} else if(isOperator(curToken) && isOperator(stack.top()->type)){
			AstNode *newNode = new AstNode{
				tokenToUnaryOperation(curToken.type),
				UnaryOperation{
					new AstNode,
				}
			};
			while(!stack.empty() && getRbp(stack.top()->type) > getLbp(newNode->type)){
				stack.pop();
			}
			stack.push(newNode);
		} else if(isOperator(curToken)){
				
		} else if(curToken.type == TokenType::identifier){
			AstNode *newNode = new AstNode{
				NodeType::identifier,
				Identifier{ curToken.text }
			};
		}
		tokenInd++;
	}
	return stack.top();
}
AbstractSyntaxTree Parser::parse(std::vector<Token> tokenStream){
	tokens.insert(tokens.end(), tokenStream.begin(), tokenStream.end());
	AstNode *exp = handleExpression(TokenType::newline);	
	return AbstractSyntaxTree(exp);
}