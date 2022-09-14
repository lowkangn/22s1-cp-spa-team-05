#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class BracketsASTNode : public ASTNode {
public:
	BracketsASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::BRACKETS;
	};

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getInBrackets() {
		// Ensure Bracket Node has only 1 children
		assert(this->getChildren().size() == 1);
		return this->children[0];
	}

	Entity extractEntity() override;
};