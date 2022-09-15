#include <sp/dataclasses/ast/StatementListASTNode.h>

Entity StatementListASTNode::extractEntity() {
	return Entity::createStmtLstEntity();
}
