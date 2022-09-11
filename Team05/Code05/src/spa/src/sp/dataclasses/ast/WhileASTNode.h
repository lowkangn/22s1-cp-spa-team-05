#pragma once

#include <sp/dataclasses/ast/AST.h>

class WhileASTNode : public ASTNode {
public:
	WhileASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::WHILE;
	}

	shared_ptr<ASTNode> getCondition() {
		return this->children[0];
	}

	shared_ptr<ASTNode> getStmtList() {
		return this->children[1];
	}

	Entity extractEntity() override;
};