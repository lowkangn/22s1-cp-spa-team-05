#pragma once

#include <sp/dataclasses/ast/AST.h>

class IfASTNode : public ASTNode {
private:
	IfASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::IF;
	}
public:
	static shared_ptr<ASTNode> createIfNode() {
		return shared_ptr<ASTNode>(new IfASTNode(Token::createIfToken()));
	}

	bool isTerminal() override {
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