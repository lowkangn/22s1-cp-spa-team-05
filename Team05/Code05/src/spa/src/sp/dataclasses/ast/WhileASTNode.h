#pragma once

#include <sp/dataclasses/ast/AST.h>

class WhileASTNode : public ASTNode {
public:
	WhileASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::WHILE;
	}

	shared_ptr<ASTNode> getCondition() {
		// Ensure While node only has 2 children
		assert(this->getChildren().size() == 2);
		return this->children[0];
	}

	shared_ptr<ASTNode> getStmtList() {
		// Ensure While node only has 2 children
		assert(this->getChildren().size() == 2);
		return this->children[1];
	}

	bool isTerminal() override {
		return false;
	}

	Entity extractEntity() override;
};