#include <sp/dataclasses/ast/PrintASTNode.h>

Entity PrintASTNode::extractEntity() {
	int lineNumber = this->getLineNumber();
	return Entity::createPrintEntity(lineNumber);
}
