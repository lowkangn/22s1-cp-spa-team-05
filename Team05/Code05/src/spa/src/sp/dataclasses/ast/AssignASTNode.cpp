#include <sp/dataclasses/ast/AssignASTNode.h>

Entity AssignASTNode::extractEntity() {
	Token assignToken = this->getToken();
	int lineNumber = this->getLineNumber();
	return Entity{ EntityType::STMT, lineNumber, assignToken };
}
