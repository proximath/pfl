#pragma once

#include "../token/token.hpp"
#include "../ast/astnode.hpp"

static ExprKind tokenToBinaryOperator(TokenType type){
	switch(type){
	case TokenType::plus:
		return ExprKind::addition;
	case TokenType::minus:
		return ExprKind::subtraction;
	case TokenType::asterisk:
		return ExprKind::multiplication;
	case TokenType::slash:
		return ExprKind::division;
	case TokenType::exponent:
		return ExprKind::exponentiation;
	case TokenType::doubleAmpersand:
		return ExprKind::conjunction;
	case TokenType::doubleBar:
		return ExprKind::disjunction;
	case TokenType::equal:
		return ExprKind::assignment;
	case TokenType::doubleEqual:
		return ExprKind::equality;
	case TokenType::notEqual:
		return ExprKind::inequality;
	case TokenType::less:
		return ExprKind::lessThan;
	case TokenType::more:
		return ExprKind::greaterThan;
	case TokenType::lessEqual:
		return ExprKind::lessEqual;
	case TokenType::moreEqual:
		return ExprKind::greaterEqual;
	case TokenType::dot:
		return ExprKind::memberAccess;
	case TokenType::andKeyword:
		return ExprKind::conjunction;
	case TokenType::orKeyword:
		return ExprKind::disjunction;
	case TokenType::parenStart:
		return ExprKind::call;
	case TokenType::squareStart:
		return ExprKind::arrayAccess;
	default:
		throw SystemError("tokenToBinaryOperator not a binary operator", __FILE_NAME__, __LINE__);
	}
}

static ExprKind tokenToUnaryOperation(TokenType type){
	switch(type){
	case TokenType::plus:
		return ExprKind::plusSign;
	case TokenType::minus:
		return ExprKind::minusSign;
	case TokenType::notKeyword:
		return ExprKind::negation;
	default:
		throw SystemError(std::string("tokenToUnaryOperator ") + getTokenTypeName(type) + " not a unary operator", __FILE_NAME__, __LINE__);
	}
}

static ExprNode* tokenToPrimary(Token &token){
	switch(token.type){
	case TokenType::identifier:
		return new ExprNode(ExprKind::identifier, Identifier{token.text});
	case TokenType::intLiteral:
		return new ExprNode(ExprKind::intLiteral, IntLiteral{token.text});
	case TokenType::floatLiteral:
		return new ExprNode(ExprKind::floatLiteral, FloatLiteral{token.text});
	case TokenType::trueKeyword:
		return new ExprNode(ExprKind::boolLiteral, BoolLiteral{token.text});
	case TokenType::falseKeyword:
		return new ExprNode(ExprKind::boolLiteral, BoolLiteral{token.text});
	case TokenType::string:
		return new ExprNode(ExprKind::stringLiteral, StringLiteral{token.text});
	case TokenType::formatString:
		return new ExprNode(ExprKind::formatString, FormatString{
			{new ExprNode(ExprKind::stringLiteral, StringLiteral{token.text})}
		});
	default:
		throw SystemError("tokenToPrimary not a primary", __FILE_NAME__, __LINE__);
	}
}

// Note: There's a node version as well
static bool isPrimary(const TokenType type){
    return 
    type == TokenType::intLiteral || 
    type == TokenType::floatLiteral || 
    type == TokenType::trueKeyword || 
    type == TokenType::falseKeyword || 
    type == TokenType::string ||
    type == TokenType::identifier ||
	type == TokenType::formatString;
}

static bool isPrimary(const Token &token){
    return isPrimary(token.type);
}

static bool isOpeningBrace(const TokenType type){
    return
    type == TokenType::parenStart;
}

static bool isOpeningBrace(const Token &token){
    return isOpeningBrace(token.type);
}

static TokenType getMatchingBrace(const TokenType braceType){
    switch(braceType){
    case TokenType::parenStart:
        return TokenType::parenEnd;
    case TokenType::curlyStart:
        return TokenType::curlyEnd;
    case TokenType::squareStart:
        return TokenType::squareEnd;
    default:
        throw SystemError("At getMatchingBrace not a brace", __FILE_NAME__, __LINE__);
    }
}

// Note: there's a token version as well
static bool isPrimary(ExprKind type){
    return 
    type == ExprKind::identifier ||
    type == ExprKind::intLiteral ||
    type == ExprKind::floatLiteral ||
    type == ExprKind::stringLiteral ||
    type == ExprKind::boolLiteral ||
	type == ExprKind::formatString ||
    type == ExprKind::callArgsList ||
    type == ExprKind::arrayLiteral ||
    type == ExprKind::arraySubscript;
}

