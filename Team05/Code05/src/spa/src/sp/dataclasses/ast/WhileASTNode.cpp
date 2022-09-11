#include <sp/dataclasses/ast/WhileASTNode.h>

Entity WhileASTNode::extractEntity() {
	Token whileToken = this->getToken();
	int lineNumber = this->getLineNumber();
	return Entity{ EntityType::WHILE, lineNumber, whileToken };
}
