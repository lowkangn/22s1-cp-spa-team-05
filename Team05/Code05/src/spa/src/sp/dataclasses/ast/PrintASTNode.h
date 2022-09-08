#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class PrintASTNode : public ASTNode {
public:
	PrintASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PRINT;
	};

	Entity extractEntity() override;
};