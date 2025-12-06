#include "../include/parser.hpp"
#include "parser-utils.hpp"
#include "../token/operator.hpp"
#include "../ast/operator.hpp"

#include <algorithm>

void Parser::popOperatorStack(std::vector<AstNode*> &operatorNodes, AstNode *&lastPrimary, AstNode *&newNode){
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
}

AstNode* Parser::handleExpression(std::vector<TokenType> delimeters){
	std::cout << "EXP" << std::endl;
	AstNode *lastPrimary = nullptr;
	bool prevOperator = false;
	bool prevUnary = false;
	std::vector<AstNode*> operatorNodes;
		
	//AstNode *assignment = tryAssignment({ TokenType::newline });
	// if(assignment){
	// 	return assignment;
	// }
	while(tokenInd < tokens.size()){
		Token &curToken = tokens[tokenInd];
		std::cout << "Reading token " << getTokenTypeName(curToken.type) << std::endl;
		// for(int i = 0; i < operatorNodes.size(); i++){
		// 	std::cout << getNodeTypeName(operatorNodes[i]->type) << " ";
		// }
		// std::cout << std::endl;
		//std::cout << (std::find(delimeter.begin(), delimeter.end(), curToken.type)) << " " << (delimeter.end()) << std::endl;
		if(std::find(delimeters.begin(), delimeters.end(), curToken.type) != delimeters.end()){
			//std::cout << "HEY" << std::endl;
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
			//std::cout << "Add binary/postfix operator" << std::endl;
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
			popOperatorStack(operatorNodes, lastPrimary, newNode);
			lastPrimary = nullptr;
			prevUnary = false;
			prevOperator = true;
		} else if(isPrimary(curToken)){
			if(lastPrimary){
				//std::cerr << getNodeTypeName(lastPrimary->type) << " " << lastPrimary->as<Identifier>().name << " "
				// << curToken.text << " " << curToken.line_num << " " << curToken.column_num << std::endl;
				emitError("Expected an operator");
			}
			AstNode *newNode = tokenToPrimary(curToken); 			
			lastPrimary = newNode;
			prevOperator = false;
			prevUnary = false;
		} else if(curToken.type == TokenType::parenStart){
			if(lastPrimary){
				AstNode *callNode = new AstNode(NodeType::call, BinaryOperation{});
				AstNode *args = handleCallArgsList();
				popOperatorStack(operatorNodes, lastPrimary, callNode);
				lastPrimary = args;
			} else {
				tokenInd++;
				lastPrimary = handleExpression({ TokenType::parenEnd });
			}
			prevOperator = false;
			prevUnary = false;
			continue;
		} else if(curToken.type == TokenType::fnKeyword){
			AstNode *func = handleFn();
			return func;
		} else if(curToken.type == TokenType::ifKeyword){
			AstNode *expr = handleIf();
			return expr;
		} else if(curToken.type == TokenType::squareStart){
			if(lastPrimary){
				AstNode *accessNode = new AstNode(NodeType::arrayAccess, BinaryOperation{});	
				AstNode *subscript = handleArraySubscript();
				popOperatorStack(operatorNodes, lastPrimary, accessNode);
				lastPrimary = subscript;
			} else {
				AstNode *array = handleArrayLiteral();
				lastPrimary = array;
			}
			prevOperator = false;
			prevUnary = false;
			continue;
		}
		tokenInd++;
	}
	//std::cout << "RET" << std::endl;
	if(!operatorNodes.empty()){
		return operatorNodes.front();
	} else if(lastPrimary){
		return lastPrimary;	
	} else {
		return nullptr;
	}
}
