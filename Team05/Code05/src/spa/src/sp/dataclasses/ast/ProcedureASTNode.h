#pragma once

#include <sp/dataclasses/ast/ProcedureASTNode.h>

class ProcedureASTNode : public ASTNode {
public:
	ProcedureASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PROCEDURE;
	}

	Entity extractEntity() override;
};