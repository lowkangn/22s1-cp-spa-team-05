#pragma once

#include <sp/dataclasses/ast/AST.h>

class AssignASTNode : public ASTNode {
public:
	AssignASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::ASSIGN;
	}

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getLeftHandSide() {
		// LHS is stored in index 0
		return this->children[0];
	}

	shared_ptr<ASTNode> getRightHandSide() {
		// RHS is stored in index 1
		return this->children[1];
	}

	Entity extractEntity() override;
};