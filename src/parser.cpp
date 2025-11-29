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
	default:
		throw SystemError("tokenToUnaryOperator not a unary operator", __FILE_NAME__, __LINE__);
	}
}

NodeType tokenToPrimary(TokenType type){
	switch(type){
	case TokenType::identifier:
		return NodeType::identifier;
	case TokenType::intLiteral:
		return NodeType::intLiteral;
	case TokenType::floatLiteral:
		return NodeType::floatLiteral;
	case TokenType::string:
		return NodeType::stringLiteral;
	default:
		throw SystemError("tokenToPrimary not a primary", __FILE_NAME__, __LINE__);
	}
}

void Parser::emitError(const std::string &msg){
	throw ParserError(msg);
}

AstNode* Parser::handleExpression(TokenType delimeter, int minBp){
	std::cout << "New exp" << std::endl;
	AstNode *lastPrimary = nullptr;
	bool prevOperator = false;
	bool prevUnary = false;
	std::vector<AstNode*> operatorNodes;

	while(tokenInd < tokens.size()){
		Token &curToken = tokens[tokenInd];
		std::cout << "Reading token " << tokenTypeName(curToken.type) << std::endl;
		for(int i = 0; i < operatorNodes.size(); i++){
			std::cout << getNodeTypeName(operatorNodes[i]->type) << " ";
		}
		std::cout << std::endl;
		if(curToken.type == delimeter){
			if(!operatorNodes.empty() && lastPrimary){
				AstNode *lastOp = operatorNodes.back();
				if(isPrefixOperator(lastOp->type)){
					lastOp->as<UnaryOperation>().expr = lastPrimary;
				} else if(isBinaryOperator(lastOp->type)){
					lastOp->as<BinaryOperation>().right = lastPrimary;
				}
			}
			while(operatorNodes.size() >= 2){
				AstNode *last = operatorNodes.back();
				AstNode *secLast = operatorNodes.at(operatorNodes.size() - 2);
				if(isPrefixOperator(secLast->type)){
					secLast->as<UnaryOperation>().expr = last;
				} else {
					secLast->as<BinaryOperation>().right = last;
				}
				operatorNodes.pop_back();
			}
			tokenInd++;
			break;
		} else if(isOperator(curToken) && (!lastPrimary || prevOperator)){ // Prefix Operator
			AstNode *newNode = new AstNode(
				tokenToUnaryOperation(curToken.type),
				UnaryOperation()
			);
			operatorNodes.push_back(newNode);
			lastPrimary = nullptr;
			prevUnary = true;
			prevOperator = true;
		} else if(isOperator(curToken)){ // Binary/Postfix Operator
			std::cout << "Add binary/postfix operator" << std::endl;
			AstNode *newNode; 
			if(isPostfixOp(curToken.type)){
				newNode = new AstNode(
					tokenToUnaryOperation(curToken.type),
					UnaryOperation{}
				);
			} else {
				newNode = new AstNode(
					tokenToBinaryOperator(curToken.type),
					BinaryOperation{}
				);
			}
			if(!operatorNodes.empty() && getRbp(operatorNodes.back()->type) <= getLbp(newNode->type)){
				newNode->as<BinaryOperation>().left = lastPrimary;
			} else if(!operatorNodes.empty() && isBinaryOperator(operatorNodes.back()->type)) {
				operatorNodes.back()->as<BinaryOperation>().right = lastPrimary;
			} else if(!operatorNodes.empty() && isPrefixOperator(operatorNodes.back()->type)) {
				operatorNodes.back()->as<UnaryOperation>().expr = lastPrimary;
			}
			while(operatorNodes.size() >= 2){
				AstNode *last = operatorNodes.back();
				AstNode *secLast = operatorNodes.at(operatorNodes.size() - 2);
				if(getRbp(last->type) <= getLbp(newNode->type)){
					break;
				} else if(getRbp(secLast->type) < getLbp(newNode->type)){
					newNode->as<BinaryOperation>().left = last;
					operatorNodes.pop_back();
				} else {
					secLast->as<BinaryOperation>().right = last;
					operatorNodes.pop_back();
				}
			}
			if(operatorNodes.size() == 1 && getRbp(operatorNodes.back()->type) > getLbp(newNode->type)){
				newNode->as<BinaryOperation>().left = operatorNodes.back();
				operatorNodes.pop_back();
			} else if(operatorNodes.empty()){
				newNode->as<BinaryOperation>().left = lastPrimary;
			}
			operatorNodes.push_back(newNode);
			lastPrimary = nullptr;
			prevUnary = false;
			prevOperator = true;
		} else if(isPrimary(curToken)){
			if(lastPrimary){
				emitError("Expected an operator");
			}
			AstNode *newNode = new AstNode(
				tokenToPrimary(curToken.type),
				Primary{curToken.text}
			);
			lastPrimary = newNode;
			prevOperator = false;
			prevUnary = false;
		} else if(isOpeningBrace(curToken)){
			tokenInd++;
			lastPrimary = handleExpression(getMatchingBrace(curToken.type), 0);
			AbstractSyntaxTree(lastPrimary).print(lastPrimary);
			std::cout << getNodeTypeName(lastPrimary->type) << std::endl;
			std::cout << tokenInd << std::endl;
			prevOperator = false;
			prevUnary = false;
			continue;
		}
		tokenInd++;

	}
	if(!operatorNodes.empty()){
		return operatorNodes.front();
	} else if(lastPrimary){
		return lastPrimary;	
	} else {
		return nullptr;
	}
}
AbstractSyntaxTree Parser::parse(std::vector<Token> tokenStream){
	tokens.insert(tokens.end(), tokenStream.begin(), tokenStream.end());
	AstNode *exp = handleExpression(TokenType::newline, 0);	
	std::cout << "Finished parsing" << std::endl;
	return AbstractSyntaxTree(exp);
}