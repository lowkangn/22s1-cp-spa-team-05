#include <sp/dataclasses/ast/PrintASTNode.h>

Entity PrintASTNode::extractEntity() {
	Token token = this->getToken();
	int lineNumber = this->getLineNumber();
	return Entity{ EntityType::STMT, lineNumber, token };
}
