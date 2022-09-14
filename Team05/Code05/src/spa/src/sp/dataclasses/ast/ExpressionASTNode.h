#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <assert.h>


class ExpressionASTNode : public ASTNode {
public:
	ExpressionASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::EXPRESSION;
	};

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getLeftHandSide() {
		// Ensure Expression Node has only 2 children
		assert(this->getChildren().size() == 2);
		// Should be a variable
		return this->children[0];
	}

	shared_ptr<ASTNode> getRightHandSide() {
		// Ensure Expression Node has only 2 children
		assert(this->getChildren().size() == 2);
		// The right hand side of the operator could be a variable or another expression
		return this->children[1];
	}

	Entity extractEntity() override;
};