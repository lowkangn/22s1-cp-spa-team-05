#include <sp/dataclasses/ast/CallASTNode.h>

Entity CallASTNode::extractEntity() {
	Token callToken = this->getToken();
	int lineNumber = this->getLineNumber();
	return Entity{ EntityType::CALL, lineNumber, callToken };
}
