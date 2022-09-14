#include <sp/dataclasses/ast/StatementListASTNode.h>

Entity StatementListASTnode::extractEntity() {
	return Entity::createStmtLstEntity();
}
