#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class ReadASTNode : public ASTNode {
public:
	ReadASTNode (Token& token) : ASTNode(token) {
		this->type = ASTNodeType::READ;
	};

	bool isTerminal() override {
		return false;
	}

	shared_ptr<ASTNode> getVariableToRead() {
		// Ensure Read Node has only 1 children
		assert(this->getChildren().size() == 1);
		// Ensure Read Node only has variable node as child
		assert(this->children[0]->getType() == ASTNodeType::VARIABLE);
		return this->children[0];
	}

	Entity extractEntity() override;
};