#include "../headers/parser.hpp"

#include <algorithm>

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
		emitError(std::string("Expected token " + tokenTypeName(type)));
	}
	return tokens[tokenInd++];
}

AstNode* Parser::handleFnParamList(){
	AstNode *returned = new AstNode(NodeType::fnParamList, FnParamList{});
	expectToken(TokenType::parenStart);
	while(tokenInd < tokens.size()){
		if(getCurToken().type == TokenType::parenEnd){
			break;
		}
		Token &name = expectToken(TokenType::identifier);
		returned->as<FnParamList>().params.push_back(
			new AstNode(NodeType::identifier, Identifier{name.text})
		);
		if(getCurToken().type == TokenType::parenEnd){
			break;
		}
		expectToken(TokenType::comma);
	}
	expectToken(TokenType::parenEnd);
	return returned;
}

AstNode* Parser::handleBlock(){
	AstNode *returned = new AstNode(NodeType::block, Block{});
	while(tokenInd < tokens.size()){
		AstNode *exp = handleExpression({ TokenType::newline });
		returned->as<Block>().expressions.push_back(exp);
		if(discardToken(TokenType::dedent)){
			break;
		}
	}
	return returned;
}

AstNode* Parser::handleIf(){
	AstNode *returned = new AstNode(NodeType::ifExpr, IfExpr{});
	expectToken(TokenType::ifKeyword);
	AstNode *expr = handleExpression({ TokenType::colon });
	returned->as<IfExpr>().condition = expr;
	expectToken(TokenType::newline);
	expectToken(TokenType::indent);	
	AstNode *ifBlock = handleBlock();
	returned->as<IfExpr>().ifBlock = ifBlock;
	if(discardToken(TokenType::elseKeyword)){
		expectToken(TokenType::colon);
		expectToken(TokenType::newline);
		expectToken(TokenType::indent);
		AstNode *elseBlock = handleBlock();
		returned->as<IfExpr>().elseBlock = elseBlock;
	}
	return returned;
}

AstNode* Parser::handleFn(){
	AstNode *returned = new AstNode(NodeType::function, Function{});
	expectToken(TokenType::fnKeyword);
	Token& name = expectToken(TokenType::identifier);
	returned->as<Function>().name = new AstNode(NodeType::identifier, Identifier{name.text});
	AstNode *paramList = handleFnParamList();
	returned->as<Function>().paramList = paramList;
	expectToken(TokenType::colon);
	discardToken(TokenType::newline);
	discardToken(TokenType::indent);
	AstNode *block = handleBlock();
	returned->as<Function>().block = block;
	return returned;
}

AstNode* Parser::handleCallArgsList(){
	expectToken(TokenType::parenStart);
	AstNode *returned = new AstNode(NodeType::callArgsList, CallArgsList{});
	while(getPrevToken().type != TokenType::parenEnd){
		AstNode *arg = handleExpression({ TokenType::comma, TokenType::parenEnd });	
		//std::cout << "PB " << getNodeTypeName(arg->type) << std::endl;
		returned->as<CallArgsList>().args.push_back(arg);
		//std::cout << tokenTypeName(getPrevToken().type) << std::endl;
	}
	return returned;
}

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

AstNode* Parser::handleExpression(std::vector<TokenType> delimeters){
	std::cout << "EXP" << std::endl;
	AstNode *lastPrimary = nullptr;
	bool prevOperator = false;
	bool prevUnary = false;
	std::vector<AstNode*> operatorNodes;

	while(tokenInd < tokens.size()){
		Token &curToken = tokens[tokenInd];
		std::cout << "Reading token " << tokenTypeName(curToken.type) << std::endl;
		// for(int i = 0; i < operatorNodes.size(); i++){
		// 	std::cout << getNodeTypeName(operatorNodes[i]->type) << " ";
		// }
		// std::cout << std::endl;
		//std::cout << (std::find(delimeter.begin(), delimeter.end(), curToken.type)) << " " << (delimeter.end()) << std::endl;
		bool containsDelimeter = false;
		for(TokenType delimeter : delimeters){
			std::cout << "Comparing " << tokenTypeName(delimeter) << std::endl;
			if(delimeter == curToken.type){
				containsDelimeter = true;
				break;
			}
		}
		if(containsDelimeter){
			std::cout << "HEY" << std::endl;
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
				std::cerr << getNodeTypeName(lastPrimary->type) << " " << lastPrimary->as<Identifier>().name << " "
				 << curToken.text << " " << curToken.line_num << " " << curToken.column_num << std::endl;
				emitError("Expected an operator");
			}
			AstNode *newNode = tokenToPrimary(curToken); 			
			lastPrimary = newNode;
			prevOperator = false;
			prevUnary = false;
		} else if(curToken.type == TokenType::parenStart){
			if(lastPrimary && lastPrimary->type == NodeType::identifier){
				AstNode *callNode = new AstNode(NodeType::call, Call{});
				callNode->as<Call>().funcName = lastPrimary;
				AstNode *args = handleCallArgsList();
				callNode->as<Call>().arguments = args;
				lastPrimary = callNode;
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
			AstNode *array = handleArrayLiteral();
			return array;
		}
		tokenInd++;
	}
	std::cout << "RET" << std::endl;
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
	AstNode *root = new AstNode(NodeType::block, Block{});
	while(tokenInd < tokens.size()){
		AstNode *exp = handleExpression({ TokenType::newline });	
		root->as<Block>().expressions.push_back(exp);
	}
	//std::cout << "Finished parsing" << std::endl;
	return AbstractSyntaxTree(root);
}