#include <sp/dataclasses/ast/ReadASTNode.h>

Entity ReadASTNode::extractEntity() {
	Token token = this->getToken();
	int lineNumber = this->getLineNumber();
	return Entity{ EntityType::READ, lineNumber, token };
}
