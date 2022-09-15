#pragma once

#include <sp/dataclasses/ast/AST.h>

class IfASTNode : public ASTNode {
public:
	IfASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::IF;
	}

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getCondition() {
		// Ensure If Node has only 3 children
		assert(this->getChildren().size() == 3);
		return this->getChildren()[0];
	}	
	
	shared_ptr<ASTNode> getThenStatements() {
		// Ensure If Node has only 3 children
		assert(this->getChildren().size() == 3);
		return this->getChildren()[1];
	}	
	
	shared_ptr<ASTNode> getElseStatements() {
		// Ensure If Node has only 3 children
		assert(this->getChildren().size() == 3);
		return this->getChildren()[2];
	}

	Entity extractEntity() override;
};