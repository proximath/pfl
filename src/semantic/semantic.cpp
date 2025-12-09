#include "semantic.hpp"
#include "../parser/parser-utils.hpp"
#include "semantic-utils.hpp"

SemanticAnalyzer::SemanticAnalyzer(AstNode *root){
    curNode = root;
    curScope = new Scope;
    types.push_back({ Type{ "none", true, NONE_INDEX } });
    types.push_back({ Type{ "int", true, INT_INDEX } });
    types.push_back({ Type{ "float", true, FLOAT_INDEX } });
    types.push_back({ Type{ "bool", true, BOOL_INDEX } });
    types.push_back({ Type{ "str", true, STR_INDEX } });
    types.push_back({ Type{ "<Type>", true, TYPE_INDEX } });
    curScope->symbolTable.insert({
        { "int", Symbol{ "int", SymbolKind::primitive, &types[INT_INDEX] }},
        { "float", Symbol{ "float", SymbolKind::primitive, &types[INT_INDEX] }},
        { "str", Symbol{ "str", SymbolKind::primitive, &types[INT_INDEX] }},
        { "bool", Symbol{ "bool", SymbolKind::primitive, &types[INT_INDEX] }}
    });
    analyze(curNode);
}

Type& SemanticAnalyzer::analyze(AstNode *root){
    return typeCheckExpr(root);
}

Type& SemanticAnalyzer::getPrimaryType(AstNode *primary){
    switch (primary->type) {
    case NodeType::intLiteral:
        return types[INT_INDEX];
    break;
    case NodeType::floatLiteral:
        return types[FLOAT_INDEX];
    break;
    case NodeType::falseLiteral:
        return types[BOOL_INDEX];
    break;
    case NodeType::trueLiteral:
        return types[BOOL_INDEX];
    break;
    case NodeType::arrayLiteral:
        throw SystemError("unimplemented", __FILE_NAME__, __LINE__);
    break;
    case NodeType::stringLiteral:
        return types[STR_INDEX];
    break;
    default:
        throw SystemError("SemanticAnalyzer::getPrimaryType more primary types to come");
    }
}

void SemanticAnalyzer::precomputePrimary(AstNode *primary){
    switch (primary->type) {
    case NodeType::intLiteral:
        primary->as<IntLiteral>().precomputed = std::stoi(
            primary->as<IntLiteral>().text
        );
    break;
    case NodeType::floatLiteral:
        primary->as<FloatLiteral>().precomputed = std::stod(
            primary->as<FloatLiteral>().text
        );
    break;
    }
}



bool isArithmeticType(Type &ty){
    return ty.index == INT_INDEX ||
        ty.index == FLOAT_INDEX;
}

bool isBinaryArithOperation(NodeType type){
    return type == NodeType::addition ||
        type == NodeType::subtraction ||
        type == NodeType::multiplication ||
        type == NodeType::division ||
        type == NodeType::exponentiation;
}

bool isComparisonOperation(NodeType type){
    return type == NodeType::equality ||
        type == NodeType::inequality ||
        type == NodeType::greaterThan ||
        type == NodeType::greaterEqual ||
        type == NodeType::lessThan ||
        type == NodeType::lessEqual;
}

bool isBinaryLogicOperation(NodeType type){
    return type == NodeType::conjunction ||
        type == NodeType::disjunction;
}

bool SemanticAnalyzer::isAlreadyInScope(const std::string &name){
    return curScope->symbolTable.count(name);
}

Symbol* SemanticAnalyzer::findSymbol(const std::string &name, Scope *scope){
    if(scope->symbolTable.count(name) == 0){
        if(!scope->parent){
            return nullptr;
        }
        return findSymbol(name, scope->parent);
    } else {
        return &scope->symbolTable[name];
    }
}

Type& analyzeTuplePattern(AstNode *node){

}

Type* SemanticAnalyzer::analyzeAssignment(AstNode *node){
    AstNode *lhs = node->as<Assignment>().lhs;
    AstNode *rhs = node->as<Assignment>().rhs;

}

Type& SemanticAnalyzer::typeCheckExpr(AstNode *node){
    node->valueType = &types[NONE_INDEX];
    if(isPrimary(node->type)){
        precomputePrimary(node);
        node->valueType = &getPrimaryType(node);
        return *(node->valueType);
    }
    if(isBinaryArithOperation(node->type)){
        Type &left = typeCheckExpr(node->as<BinaryOperation>().left);
        Type &right = typeCheckExpr(node->as<BinaryOperation>().right);
        if(!isArithmeticType(left)){
            emitError(
                std::string("Arithmetic operator ") + 
                getNodeTypeName(node->type) + " is not supported on type " + 
                left.name
            );
        }
        if(!isArithmeticType(right)){
            emitError(
                std::string("Arithmetic operator ") + 
                getNodeTypeName(node->type) + " is not supported on type " + 
                right.name
            );
        }
        node->valueType = promoteIfNeeded(
            &left, &right, 
            node->as<BinaryOperation>().left, 
            node->as<BinaryOperation>().right, 
            node
        );
        return *(node->valueType); 
    }
    if(isBinaryLogicOperation(node->type)){
        Type &left = typeCheckExpr(node->as<BinaryOperation>().left);
        Type &right = typeCheckExpr(node->as<BinaryOperation>().right);
        if(left.index != BOOL_INDEX){
            emitError(
                std::string("Logic operator ") + 
                getNodeTypeName(node->type) + " is not supported on type " + 
                left.name
            );
        } else if(right.index != BOOL_INDEX){
            emitError(
                std::string("Logic operator ") + 
                getNodeTypeName(node->type) + " is not supported on type " + 
                right.name
            );
        }
        node->valueType = &types[BOOL_INDEX];
        return types[BOOL_INDEX];
    }
    if(isComparisonOperation(node->type)){
        Type &left = typeCheckExpr(node->as<BinaryOperation>().left);
        Type &right = typeCheckExpr(node->as<BinaryOperation>().right);
        if(!isArithmeticType(left)){
            emitError(
                std::string("Comparison operator ") + 
                getNodeTypeName(node->type) + " is not supported on type " + 
                left.name
            );
        }
        if(!isArithmeticType(right)){
            emitError(
                std::string("Comparison operator ") + 
                getNodeTypeName(node->type) + " is not supported on type " + 
                right.name
            );
        }
        if(isImplicitCastable(left, right)){
            node->as<BinaryOperation>().left = insertImplicitCastNode(
                left, 
                right, 
                node->as<BinaryOperation>().left 
            );
        } else if(isImplicitCastable(right, left)){
            node->as<BinaryOperation>().right = insertImplicitCastNode(
                right, 
                left, 
                node->as<BinaryOperation>().right 
            );
        }
        node->valueType = &types[BOOL_INDEX];
        return types[BOOL_INDEX];
    }
    switch(node->type){
    case NodeType::block:
        for(AstNode *child : node->as<Block>().expressions){
            node->valueType = &typeCheckExpr(child);
        }
        return *(node->valueType);
    break;
    case NodeType::ifExpr:
    {
        Type &condition = typeCheckExpr(node->as<IfExpr>().condition);
        if(condition.index != BOOL_INDEX){
            emitError("Condition of if/elif must return bool");
        }
        Type &block = typeCheckExpr(node->as<IfExpr>().ifBlock);
        for(int i = 0; i < node->as<IfExpr>().elifBlock.size(); i++){
            Type &condition = typeCheckExpr(node->as<IfExpr>().elifCondition[i]);
            if(condition.index != BOOL_INDEX){
                emitError("Condition of if/elif must return bool");
            }
            Type &elifBlock = typeCheckExpr(node->as<IfExpr>().elifBlock[i]);
            if(elifBlock.index != block.index){
                emitError("For now, all expression inside if/elif/else must return the same type");
            }
        }
        Type &elseBlock = typeCheckExpr(node->as<IfExpr>().elseBlock);
        if(elseBlock.index != block.index){
            emitError("For now, all expression inside if/elif/else must return the same type");
        }
        node->valueType = &block;
        return *(node->valueType);
    }
    break;
    case NodeType::structure:
    {
        std::string &name = node->as<Structure>().name;
        if(isAlreadyInScope(name)){
            emitError("Reusing name that has been previously defined");
        }
        types.push_back(Type{ 
            name,
            false, 
            (int)types.size() 
        });
        pushNewScope();
        for(AstNode *field : node->as<Structure>().fields){
            if(isAlreadyInScope(field->as<TypedIdentifier>().name)){
                emitError("Duplicate struct field name");
            }
            Symbol *symbol = findSymbol(field->as<TypedIdentifier>().type, curScope);
            if(!symbol){
                emitError(std::string("Type ") + field->as<TypedIdentifier>().type + " does not exist");
            }
            if(!isType(*symbol)){
                emitError("Cannot use non-type symbol as type");
            }
            Type &type = *(symbol->type);
            curScope->symbolTable.insert({ 
                field->as<TypedIdentifier>().name, 
                Symbol{ 
                    field->as<TypedIdentifier>().name, 
                    SymbolKind::attribute, 
                    &type 
                }
            });
            types.back().attributes.insert({ 
                field->as<TypedIdentifier>().name,
                &type
            });
            field->valueType = &type;
        }
        popScope();
        curScope->symbolTable.insert({ 
            name,
            Symbol{ 
                name,
                SymbolKind::structure, 
                &types.back() 
            }
        });
        node->valueType = &types[TYPE_INDEX];
        return types[TYPE_INDEX];
    }
    break;
    case NodeType::assignment:
        analyzeAssignment(node);
    break;
    case NodeType::fnParamList:
        throw SystemError("unimplemented", __FILE_NAME__, __LINE__);
    case NodeType::function:
        throw SystemError("unimplemented", __FILE_NAME__, __LINE__);
        //typeCheckExpr(node->as<Function>().paramList);
    default:
        throw SystemError(std::string("SemanticAnalyzer::typeCheckExpr node type ") + 
            getNodeTypeName(node->type) + 
            " is unimplemented", __FILE_NAME__, __LINE__
        );
    }
}