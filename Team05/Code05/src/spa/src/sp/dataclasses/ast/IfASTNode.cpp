#include <sp/dataclasses/ast/IfASTNode.h>

Entity IfASTNode::extractEntity() {
	Token ifToken = this->getToken();
	int lineNumber = this->getLineNumber();

	return Entity{ EntityType::IF, lineNumber, ifToken };
}
