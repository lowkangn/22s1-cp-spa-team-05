#pragma once

#include <sp/dataclasses/ast/AST.h>

class ProcedureASTNode : public ASTNode {
private:
	ProcedureASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PROCEDURE;
	}
public:
	static shared_ptr<ASTNode> createProcedureNode(Token token) {
		return shared_ptr<ASTNode>(new ProcedureASTNode(token));
	}

	bool isTerminal() override {
		return false;
	}

	shared_ptr<ASTNode> getStmtList() {
		assert(this->children.size() == 1);
		return this->children[0];
	}

	Entity extractEntity() override;
};