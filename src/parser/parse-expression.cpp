#include "../parser/parser.hpp"
#include "parser-utils.hpp"
#include "../token/operator.hpp"
#include "../ast/operator.hpp"

#include <algorithm>

void Parser::popOperatorStack(std::vector<ExprNode*> &operatorNodes, ExprNode *&lastPrimary, ExprNode *&newNode){
	if(!operatorNodes.empty() && getRbp(operatorNodes.back()->kind) <= getLbp(newNode->kind)){
		newNode->as<BinaryOperation>().left = lastPrimary;
	} else if(!operatorNodes.empty() && isBinaryOperator(operatorNodes.back()->kind)) {
		operatorNodes.back()->as<BinaryOperation>().right = lastPrimary;
	} else if(!operatorNodes.empty() && isPrefixOperator(operatorNodes.back()->kind)) {
		operatorNodes.back()->as<UnaryOperation>().expr = lastPrimary;
	}
	while(operatorNodes.size() >= 2){
		ExprNode *last = operatorNodes.back();
		ExprNode *secLast = operatorNodes.at(operatorNodes.size() - 2);
		if(getRbp(last->kind) <= getLbp(newNode->kind)){
			break;
		} else if(getRbp(secLast->kind) < getLbp(newNode->kind)){
			newNode->as<BinaryOperation>().left = last;
			operatorNodes.pop_back();
		} else {
			secLast->as<BinaryOperation>().right = last;
			operatorNodes.pop_back();
		}
	}
	if(operatorNodes.size() == 1 && getRbp(operatorNodes.back()->kind) > getLbp(newNode->kind)){
		newNode->as<BinaryOperation>().left = operatorNodes.back();
		operatorNodes.pop_back();
	} else if(operatorNodes.empty()){
		newNode->as<BinaryOperation>().left = lastPrimary;
	}
    operatorNodes.push_back(newNode);
}

ExprNode* Parser::handleExpression(std::vector<TokenType> delimeters){
	//std::cout << "EXP" << std::endl;
	ExprNode *lastPrimary = nullptr;
	bool prevOperator = false;
	bool prevUnary = false;
	std::vector<ExprNode*> operatorNodes;
		
	//ExprNode *assignment = tryAssignment({ TokenType::newline });
	// if(assignment){
	// 	return assignment;
	// }
	while(tokenInd < tokens.size()){
		Token &curToken = tokens[tokenInd];
		//std::cout << "Reading token " << getTokenTypeName(curToken.type) << std::endl;
		// for(int i = 0; i < operatorNodes.size(); i++){
		// 	std::cout << ExprKind(operatorNodes[i]->type) << " ";
		// }
		// std::cout << std::endl;
		//std::cout << (std::find(delimeter.begin(), delimeter.end(), curToken.type)) << " " << (delimeter.end()) << std::endl;
		if(std::find(delimeters.begin(), delimeters.end(), curToken.type) != delimeters.end()){
			//std::cout << "HEY" << std::endl;
			if(!operatorNodes.empty() && lastPrimary){
				ExprNode *lastOp = operatorNodes.back();
				if(isPrefixOperator(lastOp->kind)){
					lastOp->as<UnaryOperation>().expr = lastPrimary;
				} else if(isBinaryOperator(lastOp->kind)){
					lastOp->as<BinaryOperation>().right = lastPrimary;
				}
			}
			while(operatorNodes.size() >= 2){
				ExprNode *last = operatorNodes.back();
				ExprNode *secLast = operatorNodes.at(operatorNodes.size() - 2);
				if(isPrefixOperator(secLast->kind)){
					secLast->as<UnaryOperation>().expr = last;
				} else {
					secLast->as<BinaryOperation>().right = last;
				}
				operatorNodes.pop_back();
			}
			tokenInd++;
			break;
		} else if(isOperator(curToken) && (!lastPrimary || prevOperator)){ // Prefix Operator
			ExprNode *newNode = new ExprNode(
				tokenToUnaryOperation(curToken.type),
				UnaryOperation()
			);
			operatorNodes.push_back(newNode);
			lastPrimary = nullptr;
			prevUnary = true;
			prevOperator = true;
		} else if(isOperator(curToken)){ // Binary/Postfix Operator
			//std::cout << "Add binary/postfix operator" << std::endl;
			ExprNode *newNode; 
			if(isPostfixOp(curToken.type)){
				newNode = new ExprNode(
					tokenToUnaryOperation(curToken.type),
					UnaryOperation{}
				);
			} else {
				newNode = new ExprNode(
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
				//std::cerr << ExprKind(lastPrimary->type) << " " << lastPrimary->as<Identifier>().name << " "
				// << curToken.text << " " << curToken.line_num << " " << curToken.column_num << std::endl;
				emitError("Expected an operator");
			}
			ExprNode *newNode = tokenToPrimary(curToken); 			
			lastPrimary = newNode;
			prevOperator = false;
			prevUnary = false;
		} else if(curToken.type == TokenType::parenStart){
			if(lastPrimary){
				ExprNode *callNode = new ExprNode(ExprKind::call, BinaryOperation{});
				ExprNode *args = handleCallArgsList();
				popOperatorStack(operatorNodes, lastPrimary, callNode);
				lastPrimary = args;
			} else {
				tokenInd++;
				lastPrimary = handleExpression({ TokenType::parenEnd });
			}
			prevOperator = false;
			prevUnary = false;
			continue;
		} else if(curToken.type == TokenType::doubleQuote){
			ExprNode *format = handleFormatString();
			lastPrimary = format;
			prevOperator = false;
			prevUnary = false;
			continue;
		} else if(curToken.type == TokenType::fnKeyword){
			ExprNode *func = handleFn();
			return func;
		} else if(curToken.type == TokenType::ifKeyword){
			ExprNode *expr = handleIf();
			return expr;
		} else if(curToken.type == TokenType::structKeyword){
			ExprNode *structure = handleStruct();
			return structure;		
		} else if(curToken.type == TokenType::enumKeyword){
			ExprNode *enumeration = handleEnum();
			return enumeration;
		} else if(curToken.type == TokenType::matchKeyword){
			ExprNode *match = handleMatch();
			return match;
		} else if(curToken.type == TokenType::squareStart){
			if(lastPrimary){
				ExprNode *accessNode = new ExprNode(ExprKind::arrayAccess, BinaryOperation{});	
				ExprNode *subscript = handleArraySubscript();
				popOperatorStack(operatorNodes, lastPrimary, accessNode);
				lastPrimary = subscript;
			} else {
				ExprNode *array = handleArrayLiteral();
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
