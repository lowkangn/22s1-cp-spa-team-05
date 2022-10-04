#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <assert.h>

class AssignASTNode : public ASTNode {
private:
	AssignASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::ASSIGN;
	}

public:
	static shared_ptr<ASTNode> createAssignNode() {
		return shared_ptr<ASTNode>(new AssignASTNode(Token::createEqualsToken()));
	}

	bool isTerminal() override {
		return false;
	}

	shared_ptr<ASTNode> getLeftHandSide() {
		// Ensure that AssignNode has only 2 children LHS and RHS
		assert(this->getChildren().size() == 2);
		// LHS is stored in index 0
		return this->children[0];
	}

	shared_ptr<ASTNode> getRightHandSide() {
		// Ensure that AssignNode has only 2 children LHS and RHS
		assert(this->getChildren().size() == 2);
		// RHS is stored in index 1
		return this->children[1];
	}

	Entity extractEntity() override;
};