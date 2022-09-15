#pragma once

#include <sp/dataclasses/ast/ProcedureASTNode.h>

class ProcedureASTNode : public ASTNode {
public:
	ProcedureASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PROCEDURE;
	}

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getStmtLst() {
		assert(this->children.size() == 1);
		return this->children[0];
	}

	Entity extractEntity() override;
};