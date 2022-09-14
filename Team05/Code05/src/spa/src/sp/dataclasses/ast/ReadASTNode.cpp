#include <sp/dataclasses/ast/ReadASTNode.h>

Entity ReadASTNode::extractEntity() {
	int lineNumber = this->getLineNumber();
	return Entity::createReadEntity(lineNumber);
}
