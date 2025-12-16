#include "../parser/parser.hpp"
#include "parser-utils.hpp"
#include "../token/operator.hpp"
#include "../ast/operator.hpp"
#include "../ast/print.hpp"

ExprNode* Parser::handleBlock(){
	ExprNode *returned = new ExprNode(ExprKind::block, Block{});
	ExprNode *assignment = tryAssignment();
	if(assignment){
		returned->as<Block>().expressions.push_back(assignment);
	} else {
		//std::cout << "FAIL" << std::endl;
	}
	while(tokenInd < tokens.size()){
		ExprNode *exp = handleExpression({ TokenType::newline });
		returned->as<Block>().expressions.push_back(exp);
		if(discardToken(TokenType::dedent)){
			break;
		}
	}
	return returned;
}

ExprNode* Parser::handleStringTemplate(){
	ExprNode *returned = new ExprNode(ExprKind::stringTemplate, StringTemplate{});
	ExprNode *value = handleExpression({ TokenType::curlyEnd, TokenType::colon });
	returned->as<StringTemplate>().value = value;
	//std::cout << getTokenTypeName(getPrevToken().type) << std::endl;
	if(getPrevToken().type == TokenType::colon){
		//std::cout << "YES" << std::endl;
		ExprNode *format = handleExpression({ TokenType::curlyEnd });
		//std::cout << getTokenTypeName(getCurToken().type) << std::endl;

		returned->as<StringTemplate>().format = format;
	}
	//printAst(returned);
	return returned;
}

ExprNode* Parser::handleFormatString(){
	expectToken(TokenType::doubleQuote);
	ExprNode *returned = new ExprNode(ExprKind::formatString, FormatString{});
	bool odd = true;
	while(tokenInd < tokens.size()){
		//std::cout << "READ " << getTokenTypeName(getCurToken().type) << std::endl;
		if(discardToken(TokenType::doubleQuote)) {
			break;
		}
		if(odd){
			ExprNode *child = new ExprNode(
				ExprKind::stringLiteral, 
				StringLiteral{expectToken(TokenType::formatString).text}
			);
			returned->as<FormatString>().children.push_back(child);
		} else {
			expectToken(TokenType::curlyStart);
			ExprNode *templ = handleStringTemplate();
			returned->as<FormatString>().children.push_back(templ);
		}
		odd = !odd;
	}
	//std::cout << "RET" << std::endl;
	//printAst(returned);
	return returned;
}

ExprNode* Parser::handleIf(){
	ExprNode *returned = new ExprNode(ExprKind::ifExpr, IfExpr{});
	expectToken(TokenType::ifKeyword);
	ExprNode *expr = handleExpression({ TokenType::colon });
	returned->as<IfExpr>().condition = expr;
	expectToken(TokenType::newline);
	expectToken(TokenType::indent);	
	ExprNode *ifBlock = handleBlock();
	returned->as<IfExpr>().ifBlock = ifBlock;
	while(discardToken(TokenType::elifKeyword)){
		expr = handleExpression({ TokenType::colon });
		discardToken(TokenType::newline);
		returned->as<IfExpr>().elifCondition.push_back(expr);
		ExprNode *elifBlock = handleBlock();
		returned->as<IfExpr>().elifBlock.push_back(elifBlock);
	}
	if(discardToken(TokenType::elseKeyword)){
		expectToken(TokenType::colon);
		expectToken(TokenType::newline);
		expectToken(TokenType::indent);
		ExprNode *elseBlock = handleBlock();
		returned->as<IfExpr>().elseBlock = elseBlock;
	}
	return returned;
}

FunctionParam* Parser::handleFnParam(){
	FunctionParam *param = new FunctionParam;
	param->name = expectToken(TokenType::identifier).text;
	if(discardToken(TokenType::colon)){
		param->type = handleTypeNode();
	}
	return param;
}

void Parser::handleFnParamList(Function *fn){
	while(tokenInd < tokens.size()){
		if(getCurToken().type == TokenType::parenEnd){
			break;
		}
		fn->params.push_back(handleFnParam());
		if(getCurToken().type == TokenType::parenEnd){
			break;
		}
		expectToken(TokenType::comma);
	}
	expectToken(TokenType::parenEnd);
}

void Parser::handleGenericDecl(Structure *structure){
	bool first = true;
	while(tokenInd < tokens.size()){
		if(!first){
			expectToken(TokenType::comma);
		}
		first = false;
		structure->generics.push_back(expectToken(TokenType::identifier).text);
		if(getCurToken().type == TokenType::squareEnd){
			tokenInd++;
			break;
		}
	}
}

TypeNode* Parser::handleTypeNode(){
	TypeNode *returned = new TypeNode;
	returned->main = expectToken(TokenType::identifier).text;
	if(discardToken(TokenType::squareStart)){
		bool first = true;
		while(tokenInd < tokens.size()){
			if(!first){
				expectToken(TokenType::comma);
			}
			first = false;
			returned->generics.push_back(handleTypeNode());
			if(getCurToken().type == TokenType::squareEnd){
				tokenInd++;
				break;
			} 
		}
	}
	return returned;
}

ExprNode* Parser::handleFn(){
	ExprNode *returned = new ExprNode(ExprKind::function, Function{});
	expectToken(TokenType::fnKeyword);
	if(getCurToken().type == TokenType::identifier){
		returned->as<Function>().name = discardToken(TokenType::identifier)->text;
	} else {
		returned->as<Function>().name = "<unnamed>";
	}
	handleFnParamList(returned->pun<Function>());
	if(discardToken(TokenType::arrow)){
		returned->as<Function>().returnType = handleTypeNode();
	}
	expectToken(TokenType::colon);
	discardToken(TokenType::newline);
	if(discardToken(TokenType::indent)){
		ExprNode *block = handleBlock();
		returned->as<Function>().block = block;
	} else {
		std::cerr << "WARNING: currently we have no way of terminating function at specific token" << std::endl;
		ExprNode *block = handleExpression({ TokenType::newline });
		returned->as<Function>().block = block;
	}
	return returned;
}

ExprNode* Parser::handleCallArgsList(){
	expectToken(TokenType::parenStart);
	ExprNode *returned = new ExprNode(ExprKind::callArgsList, CallArgsList{});
	while(getPrevToken().type != TokenType::parenEnd){
		ExprNode *arg = handleExpression({ TokenType::comma, TokenType::parenEnd });	
		//std::cout << "PB " << ExprKind(arg->type) << std::endl;
		returned->as<CallArgsList>().args.push_back(arg);
		//std::cout << getTokenTypeName(getPrevToken().type) << std::endl;
	}
	return returned;
}

ExprNode* Parser::handleArraySubscript(){
	expectToken(TokenType::squareStart);
	ExprNode *returned = new ExprNode(ExprKind::arraySubscript, ArraySubscript{});
	returned->as<ArraySubscript>().index = handleExpression({ TokenType::squareEnd });
	return returned;
}

ExprNode* Parser::tryTypedIdentifier(){
	//std::cout << "ENTER" << std::endl;
	addCheckpoint();
	ExprNode *returned = new ExprNode(ExprKind::typedIdentifier, TypedIdentifier{});

	std::cout << getTokenTypeName(getCurToken().type) << std::endl;
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

ExprNode* Parser::tryAssignment(){
	addCheckpoint();
	ExprNode *returned = new ExprNode(ExprKind::assignment, Assignment{});
	TuplePatternBase *pattern;
	if(pattern = tryTuplePattern(TokenType::equal)){
		returned->as<Assignment>().lhs = pattern;
		ExprNode *expr;
		if(expr = tryTupleExpression(TokenType::newline)){
			returned->as<Assignment>().rhs = expr;
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

TuplePatternBase* Parser::tryTuplePattern(TokenType delimeter){
	addCheckpoint();
	TuplePatternBase *returned = new TuplePatternNode;
	while(tokenInd < tokens.size()){
		if(getCurToken().type == delimeter){
			tokenInd++;
			break;
		}  
		if(getCurToken().type == TokenType::parenStart){
			tokenInd++;
			TuplePatternBase *child;
			if(child = tryTuplePattern(TokenType::parenEnd)){
				returned->asNode()->children.push_back(child);
			}
		} else if(getCurToken().type == TokenType::identifier){
			TuplePatternLeaf *leaf = new TuplePatternLeaf;
			leaf->isLeaf = true;
			leaf->name = expectToken(TokenType::identifier).text;
			if(discardToken(TokenType::colon)){
				leaf->type = handleTypeNode();
			}
			returned->asNode()->children.push_back(leaf);
		} else {
			restoreCheckpoint();
			return nullptr;
		}
		discardToken(TokenType::comma);
	}
	commitCheckpoint();
	return returned;
}

ExprNode* Parser::tryTupleExpression(TokenType delimeter){
	addCheckpoint();
	ExprNode *returned = new ExprNode(ExprKind::tupleExpression, TupleExpression{});
	while(tokenInd < tokens.size()){
		//std::cout << "READ " << tokenInd << " "  << getTokenTypeName(getCurToken().type) << std::endl;
		ExprNode *child;
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
			if(getPrevToken().type == delimeter) break;
		}
		discardToken(TokenType::comma);
	}
	commitCheckpoint();
	return returned;
}

StructMethod* Parser::handleStructMethod(){
	StructMethod *method = new StructMethod;
	while(tokenInd < tokens.size()){
		if(getCurToken().type == TokenType::parenEnd){
			break;
		}
		method->params.push_back(handleFnParam());
		if(getCurToken().type == TokenType::parenEnd){
			break;
		}
		expectToken(TokenType::comma);
	}
	expectToken(TokenType::parenEnd);
	if(discardToken(TokenType::arrow)){
		method->returnType = handleTypeNode();
	}
	expectToken(TokenType::colon);
	discardToken(TokenType::newline);
	if(discardToken(TokenType::indent)){
		ExprNode *block = handleBlock();
		method->block = block;
	} else {
		std::cerr << "WARNING: currently we have no way of terminating function at specific token" << std::endl;
		ExprNode *block = handleExpression({ TokenType::newline });
		method->block = block;
	}
	return method;
}

StructAttribute* Parser::handleStructAttribute(){
	StructAttribute *attr = new StructAttribute;
	expectToken(TokenType::colon);
	attr->type = handleTypeNode();
	return attr;	
}

ExprNode* Parser::handleStruct(){
	//std::cout << "Y" << std::endl;
	ExprNode *returned = createNode(ExprKind::structure);
	expectToken(TokenType::structKeyword);
	returned->as<Structure>().name = expectToken(TokenType::identifier).text;;
	if(discardToken(TokenType::squareStart)){
		handleGenericDecl(returned->pun<Structure>());
	}
	expectToken(TokenType::colon);
	expectToken(TokenType::newline);
	expectToken(TokenType::indent);
	while(tokenInd < tokens.size()){
		//std::cout << getTokenTypeName(getCurToken().type) << std::endl;
		std::string name = expectToken(TokenType::identifier, 
			std::string("Unexpected token ") + getTokenTypeName(getCurToken().type)).text;
		if(discardToken(TokenType::parenStart)){
			StructMethod *method = handleStructMethod();
			method->name = name;
			returned->as<Structure>().methods.push_back(method);
		} else {
			StructAttribute *attr = handleStructAttribute();
			attr->name = name;
			returned->as<Structure>().attributes.push_back(attr);
		}
		discardToken(TokenType::newline);
		if(discardToken(TokenType::dedent)){
			break;
		}
	}
	return returned;
}

ExprNode* Parser::handleEnum(){

}

ExprNode* Parser::handleMatch(){

}