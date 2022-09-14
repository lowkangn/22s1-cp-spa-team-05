#include <sp/dataclasses/ast/IfASTNode.h>

Entity IfASTNode::extractEntity() {
	int lineNumber = this->getLineNumber();

	return Entity::createIfEntity(lineNumber);
}
