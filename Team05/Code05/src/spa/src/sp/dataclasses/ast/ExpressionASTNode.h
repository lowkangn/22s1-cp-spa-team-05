#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class ExpressionASTNode : public ASTNode {
public:
	ExpressionASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::EXPRESSION;
	};

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getLeftHandSide() {
		// Should be a variable
		return this->children[0];
	}

	shared_ptr<ASTNode> getOperator() {
		// It is a ASTNode containing the operation being performed
		return this->children[1];

	}

	shared_ptr<ASTNode> getRightHandSide() {
		// The right hand side of the operator could be a variable or another expression
		return this->children[2];
	}

	Entity extractEntity() override;
};