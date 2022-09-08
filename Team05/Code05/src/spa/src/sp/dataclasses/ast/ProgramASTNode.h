#pragma once

#include <sp/dataclasses/ast/AST.h>

class ProgramASTNode : public ASTNode {
public:
	ProgramASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PROGRAM;
	}

	Entity extractEntity() override;
};