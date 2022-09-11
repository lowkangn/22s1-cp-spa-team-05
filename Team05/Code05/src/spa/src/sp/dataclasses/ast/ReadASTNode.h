#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class ReadASTNode : public ASTNode {
public:
	ReadASTNode (Token& token) : ASTNode(token) {
		this->type = ASTNodeType::READ;
	};

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getVariableToRead() {
		// Ensure Read Node has only 1 children
		assert(this->getChildren().size() == 1);
		return this->children[0];
	}

	Entity extractEntity() override;
};