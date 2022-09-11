#include <sp/dataclasses/ast/StatementListASTNode.h>

Entity StatementListASTnode::extractEntity() {
	Token undefinedToken = Token::getPlaceHolderToken();
	return Entity{ EntityType::STMTLIST, 1, undefinedToken };
}
