#include <sp/dataclasses/ast/VariableASTNode.h>


/*
	Return the current ASTNode as an entity
*/
Entity VariableASTNode::extractEntity() {
    Token nodeToken = this->getToken();
    int lineNumber = this->getLineNumber();

    return Entity::createVariableEntity(lineNumber, nodeToken);
}
