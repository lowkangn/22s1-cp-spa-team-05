#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>

class StatementListASTNode : public ASTNode {
public:
	StatementListASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::STMTLIST;
	}

	bool isTerminal() {
		return false;
	}

	Entity extractEntity() override;
};