#include <sp/dataclasses/ast/ProgramASTNode.h>

Entity ProgramASTNode::extractEntity() {
	Token undefinedToken = Token{ "", TokenType::INVALID };
	return Entity{ EntityType::UNDEFINED, -1, undefinedToken };
}
