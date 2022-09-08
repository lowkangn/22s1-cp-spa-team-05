#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class ExpressionASTNode : public ASTNode {
public:
	ExpressionASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::EXPRESSION;
	};

	Entity extractEntity() override;
};