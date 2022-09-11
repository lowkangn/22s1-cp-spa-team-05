#include <sp/dataclasses/ast/ConstantValueASTNode.h>


Entity ConstantValueASTNode::extractEntity() {
	Token nodeToken = this->getToken();
	int lineNumber = this->getLineNumber();

	return Entity{ EntityType::CONSTANT, lineNumber, nodeToken };
}

void ConstantValueASTNode::addChild(shared_ptr<ASTNode> child) {
	throw ASTException("Cannot add child to constant value node");
}

bool ConstantValueASTNode::isTerminal() {
	return true;
}
