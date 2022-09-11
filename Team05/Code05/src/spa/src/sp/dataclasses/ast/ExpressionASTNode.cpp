#include <sp/dataclasses/ast/ExpressionASTNode.h>

Entity ExpressionASTNode::extractEntity() {
	Token undefinedToken = Token{ "", TokenType::INVALID };
	return Entity{ EntityType::EXPRESSION , 1, undefinedToken };
}
