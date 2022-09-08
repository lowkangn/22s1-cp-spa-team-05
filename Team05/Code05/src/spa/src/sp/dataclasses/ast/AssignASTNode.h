#pragma once

#include <sp/dataclasses/ast/AST.h>

class AssignASTNode : public ASTNode {
public:
	AssignASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::ASSIGN;
	}

	Entity extractEntity() override;
};