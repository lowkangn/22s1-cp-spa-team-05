#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class PrintASTNode : public ASTNode {
public:
	PrintASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PRINT;
	};

	bool isTerminal() override {
		return false;
	}

	shared_ptr<ASTNode> getVariableToPrintNode() {
		// Ensure Print Node has only 1 children
		assert(this->getChildren().size() == 1);
		return this->children[0];
	}

	Entity extractEntity() override;
};