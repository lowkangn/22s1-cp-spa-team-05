#pragma once

#include <sp/dataclasses/ast/AST.h>

class IfASTNode : public ASTNode {
public:
	IfASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::IF;
	}

	shared_ptr<ASTNode> getCondition() {
		return this->getChildren()[0];
	}	
	
	shared_ptr<ASTNode> getThenStatements() {
		return this->getChildren()[1];
	}	
	
	shared_ptr<ASTNode> getElseStatements() {
		return this->getChildren()[2];
	}

	Entity extractEntity() override;
};