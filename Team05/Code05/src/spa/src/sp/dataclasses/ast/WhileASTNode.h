#pragma once

#include <sp/dataclasses/ast/AST.h>

class WhileASTNode : public ASTNode {
public:
	WhileASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::WHILE;
	}

	Entity extractEntity() override;
};