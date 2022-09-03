#include <sp/dataclasses/AST.h>

void ASTNode::addChild(ASTNode child) {
	this->children.push_back(child);
}