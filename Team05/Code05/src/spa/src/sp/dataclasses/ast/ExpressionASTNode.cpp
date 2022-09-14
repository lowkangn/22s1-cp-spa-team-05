#include <sp/dataclasses/ast/ExpressionASTNode.h>

Entity ExpressionASTNode::extractEntity() {
	Token nodeToken = this->getToken();
	int lineNumber = this->getLineNumber();
	return Entity::createExpressionEntity(lineNumber, nodeToken);
}
