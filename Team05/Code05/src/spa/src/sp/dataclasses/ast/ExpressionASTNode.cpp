#include <sp/dataclasses/ast/ExpressionASTNode.h>

Entity ExpressionASTNode::extractEntity() {
	Token nodeToken = this->getToken();
	return Entity::createExpressionEntity(1, nodeToken);
}
