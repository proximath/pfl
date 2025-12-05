#pragma once

#include "../include/token.hpp"
#include "../include/astnode.hpp"

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
	case TokenType::doubleAmpersand:
		return NodeType::conjunction;
	case TokenType::doubleBar:
		return NodeType::disjunction;
	case TokenType::equal:
		return NodeType::assignment;
	case TokenType::doubleEqual:
		return NodeType::equality;
	case TokenType::notEqual:
		return NodeType::inequality;
	case TokenType::less:
		return NodeType::lessThan;
	case TokenType::more:
		return NodeType::greaterThan;
	case TokenType::lessEqual:
		return NodeType::lessEqual;
	case TokenType::moreEqual:
		return NodeType::greaterEqual;
	case TokenType::dot:
		return NodeType::memberAccess;
	case TokenType::andKeyword:
		return NodeType::conjunction;
	case TokenType::orKeyword:
		return NodeType::disjunction;
	case TokenType::parenStart:
		return NodeType::call;
	default:
		throw SystemError("tokenToBinaryOperator not a binary operator", __FILE_NAME__, __LINE__);
	}
}

NodeType tokenToUnaryOperation(TokenType type){
	switch(type){
	case TokenType::plus:
		return NodeType::plusSign;
	case TokenType::minus:
		return NodeType::minusSign;
	case TokenType::notKeyword:
		return NodeType::negation;
	default:
		throw SystemError("tokenToUnaryOperator not a unary operator", __FILE_NAME__, __LINE__);
	}
}

AstNode* tokenToPrimary(Token &token){
	switch(token.type){
	case TokenType::identifier:
		return new AstNode(NodeType::identifier, Identifier{token.text});
	case TokenType::intLiteral:
		return new AstNode(NodeType::intLiteral, IntLiteral{token.text});
	case TokenType::floatLiteral:
		return new AstNode(NodeType::floatLiteral, FloatLiteral{token.text});
	case TokenType::string:
		return new AstNode(NodeType::stringLiteral, StringLiteral{token.text});
	default:
		throw SystemError("tokenToPrimary not a primary", __FILE_NAME__, __LINE__);
	}
}