#pragma once

#include <sp/dataclasses/ast/AST.h>

class ProcedureASTNode : public ASTNode {
public:
	ProcedureASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PROCEDURE;
	}

	bool isTerminal() {
		return false;
	}

	Entity extractEntity() override;
};