#include <sp/dataclasses/ast/StatementListASTNode.h>

Entity StatementListASTnode::extractEntity() {
	Token undefinedToken = Token{ "", TokenType::INVALID };
	return Entity{ EntityType::UNDEFINED, 1, undefinedToken };
}
