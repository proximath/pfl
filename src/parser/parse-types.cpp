#include "parser.hpp"

StructMethod* Parser::handleStructMethod(){
	StructMethod *method = new StructMethod;
	if(discardToken(TokenType::selfKeyword)){
		method->isStatic = false;
		discardToken(TokenType::comma);
	}
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
	ExprNode *returned = createNode(ExprKind::structure);
	expectToken(TokenType::structKeyword);
	returned->as<Structure>().name = expectToken(TokenType::identifier).text;;
	if(discardToken(TokenType::squareStart)){
		handleGenericDecl(returned->as<Structure>().generics);
	}
	expectToken(TokenType::colon);
	expectToken(TokenType::newline);
	expectToken(TokenType::indent);
	while(tokenInd < tokens.size()){
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

EnumVariant* Parser::handleEnumVariant(){
	EnumVariant *attr = new EnumVariant;
	expectToken(TokenType::colon);
	attr->type = handleTypeNode();
	return attr;	
}

ExprNode* Parser::handleEnum(){
	ExprNode *returned = createNode(ExprKind::enumeration);
	expectToken(TokenType::enumKeyword);
	returned->as<Enumeration>().name = expectToken(TokenType::identifier).text;
    if(discardToken(TokenType::squareStart)){
        handleGenericDecl(returned->as<Enumeration>().generics);
    }
    expectToken(TokenType::colon);
    expectToken(TokenType::newline);
    expectToken(TokenType::indent);
    while(tokenInd < tokens.size() && !isCurToken(TokenType::dedent)){
        std::string name = expectToken(TokenType::identifier).text;
        EnumVariant *variant = handleEnumVariant();
        expectToken(TokenType::newline);
        variant->name = name;
        returned->as<Enumeration>().variants.push_back(variant);
    }
    discardToken(TokenType::dedent);
    return returned;
}

TypeNode* Parser::handleTypeNode(){
	TypeNode *returned;
	if(discardToken(TokenType::fnKeyword)){
		TypeNodeFunction *fn = new TypeNodeFunction;
		fn->kind = TypeNodeKind::function;
		expectToken(TokenType::parenStart);
		bool first = true;
		while(!discardToken(TokenType::parenEnd)){
			if(!first){
				expectToken(TokenType::comma);
			}
			first = false;
			fn->args.push_back(handleTypeNode());
		}
		expectToken(TokenType::arrow);
		fn->retType = handleTypeNode();
		returned = fn;
	} else if(discardToken(TokenType::squareStart)){
		TypeNodeArray *array = new TypeNodeArray;
		array->kind = TypeNodeKind::array;
		array->elemType = handleTypeNode();
		expectToken(TokenType::squareEnd);	
		returned = array;
    } else if(discardToken(TokenType::parenStart)){
        TypeNodeTuple *tuple = new TypeNodeTuple;
        tuple->kind = TypeNodeKind::tuple;
        bool first = true;
        while(!isCurToken(TokenType::parenEnd)){
            if(!first){
                expectToken(TokenType::comma);
            }
            first = false;
            tuple->children.push_back(handleTypeNode());
        }
        tokenInd++;
        returned = tuple;
	} else {
		TypeNodeAtom *atom = new TypeNodeAtom;
		atom->kind = TypeNodeKind::atom;
		atom->main = expectToken(TokenType::identifier).text;
		if(discardToken(TokenType::squareStart)){
			bool first = true;
			while(tokenInd < tokens.size()){
				if(!first){
					expectToken(TokenType::comma);
				}
				first = false;
				atom->generics.push_back(handleTypeNode());
				if(getCurToken().type == TokenType::squareEnd){
					tokenInd++;
					break;
				} 
			}
		}
		returned = atom;
	}
	if(discardToken(TokenType::exclamation)){
		TypeNodeResult *result = new TypeNodeResult;
		result->kind = TypeNodeKind::result;
		result->innerType = returned;
		returned = result;
	} else if(discardToken(TokenType::question)){
		TypeNodeOption *option = new TypeNodeOption;
		option->kind = TypeNodeKind::option;
		option->innerType = returned;
		returned = option;
	}	
	return returned;
}

void Parser::handleGenericDecl(std::vector<std::string> &genericList){
	bool first = true;
	while(tokenInd < tokens.size()){
		if(!first){
			expectToken(TokenType::comma);
		}
		first = false;
		genericList.push_back(expectToken(TokenType::identifier).text);
		if(getCurToken().type == TokenType::squareEnd){
			tokenInd++;
			break;
		}
	}
}
