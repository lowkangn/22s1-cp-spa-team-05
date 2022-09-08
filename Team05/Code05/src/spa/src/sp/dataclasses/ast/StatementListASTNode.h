#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>

class StatementListASTnode : public ASTNode {
public:
	StatementListASTnode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::STMTLIST;
	}

	Entity extractEntity() override;
};