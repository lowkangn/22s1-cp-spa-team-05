#include <sp/dataclasses/ast/ExpressionASTNode.h>

Entity ExpressionASTNode::extractEntity() {
	Token undefinedToken = Token{ "", TokenType::INVALID };
	return Entity{ EntityType::UNDEFINED , 1, undefinedToken };
}
