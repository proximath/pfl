#include "../include/parser.hpp"
#include "parser-utils.hpp"
#include "../token/operator.hpp"
#include "../ast/operator.hpp"

AstNode* Parser::handleBlock(){
	AstNode *returned = new AstNode(NodeType::block, Block{});
	AstNode *assignment = tryAssignment();
	if(assignment){
		returned->as<Block>().expressions.push_back(assignment);
	} else {
		//std::cout << "FAIL" << std::endl;
	}
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
	while(discardToken(TokenType::elifKeyword)){
		expr = handleExpression({ TokenType::colon });
		discardToken(TokenType::newline);
		returned->as<IfExpr>().elifCondition.push_back(expr);
		AstNode *elifBlock = handleBlock();
		returned->as<IfExpr>().elifBlock.push_back(elifBlock);
	}
	if(discardToken(TokenType::elseKeyword)){
		expectToken(TokenType::colon);
		expectToken(TokenType::newline);
		expectToken(TokenType::indent);
		AstNode *elseBlock = handleBlock();
		returned->as<IfExpr>().elseBlock = elseBlock;
	}
	return returned;
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
		//std::cout << getTokenTypeName(getPrevToken().type) << std::endl;
	}
	return returned;
}

AstNode* Parser::handleArraySubscript(){
	expectToken(TokenType::squareStart);
	AstNode *returned = new AstNode(NodeType::arraySubscript, ArraySubscript{});
	returned->as<ArraySubscript>().index = handleExpression({ TokenType::squareEnd });
	return returned;
}

AstNode* Parser::tryTypedIdentifier(){
	//std::cout << "ENTER" << std::endl;
	addCheckpoint();
	AstNode *returned = new AstNode(NodeType::typedIdentifier, TypedIdentifier{});

	//std::cout << getTokenTypeName(getCurToken().type) << std::endl;
	if(getCurToken().type == TokenType::identifier){
		//std::cout << "YES" << std::endl;
		returned->as<TypedIdentifier>().name = getCurToken().text;
		tokenInd++;
		//std::cout << getTokenTypeName(getCurToken().type) << std::endl;
		if(discardToken(TokenType::colon)){
			//std::cout << "SUCC" << std::endl;
			Token &type = expectToken(TokenType::identifier, "Expected a type after ':'");
			returned->as<TypedIdentifier>().type = type.text;
			commitCheckpoint();
			return returned;
		}
	}
	//std::cout << "FAIL" << std::endl;
	restoreCheckpoint();
	return nullptr;
}

AstNode* Parser::tryAssignment(){
	addCheckpoint();
	AstNode *returned = new AstNode(NodeType::assignment, Assignment{});
	AstNode *pattern;
	if(pattern = tryTuplePattern(TokenType::equal)){
		returned->as<Assignment>().lhs = pattern;
		if(pattern = tryTupleExpression(TokenType::newline)){
			returned->as<Assignment>().rhs = pattern;
		} else {
			emitError("Expected an expression after equal sign");
		}
	} else {
		restoreCheckpoint();
		//std::cout << "Fail" << std::endl;
		return nullptr;
	}
	commitCheckpoint();
	return returned;
}

AstNode* Parser::tryTuplePattern(TokenType delimeter){
	addCheckpoint();
	AstNode *returned = new AstNode(NodeType::tuplePattern, TuplePattern{});
	AstNode *iden;
	while(tokenInd < tokens.size()){
		if(getCurToken().type == delimeter){
			tokenInd++;
			break;
		}  
		if(getCurToken().type == TokenType::parenStart){
			tokenInd++;
			AstNode *child;
			if(child = tryTuplePattern(TokenType::parenEnd)){
				returned->as<TuplePattern>().children.push_back(child);
			}
		} else if(iden = tryTypedIdentifier()){
			returned->as<TuplePattern>().children.push_back(iden);
		} else if(getCurToken().type == TokenType::identifier){
			AstNode *leaf = new AstNode(NodeType::identifier, Identifier{});
			leaf->as<Identifier>().name = getCurToken().text;
			returned->as<TuplePattern>().children.push_back(leaf);
			tokenInd++;
		} else {
			restoreCheckpoint();
			return nullptr;
		}
		discardToken(TokenType::comma);
	}
	commitCheckpoint();
	return returned;
}

AstNode* Parser::tryTupleExpression(TokenType delimeter){
	addCheckpoint();
	AstNode *returned = new AstNode(NodeType::tupleExpression, TupleExpression{});
	while(tokenInd < tokens.size()){
		//std::cout << "READ " << tokenInd << " "  << getTokenTypeName(getCurToken().type) << std::endl;
		AstNode *child;
		if(getCurToken().type == delimeter){
			//std::cout << "STOP" << std::endl;
			tokenInd++;
			break;
		}  
		if(discardToken(TokenType::parenStart)){
			//std::cout << "OY" << std::endl;
			if(child = tryTupleExpression(TokenType::parenEnd)){
				//std::cout << "YO" << std::endl;
				returned->as<TupleExpression>().children.push_back(child);
			} else {
				child = handleExpression({ TokenType::parenEnd });
				returned->as<TupleExpression>().children.push_back(child);
			}
		} else {
			child = handleExpression({ delimeter, TokenType::comma, TokenType::newline });
			returned->as<TupleExpression>().children.push_back(child);
		}
		discardToken(TokenType::comma);
	}
	commitCheckpoint();
	return returned;
}