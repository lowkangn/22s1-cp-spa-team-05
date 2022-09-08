#pragma once

#include <sp/dataclasses/ast/AST.h>

class CallASTNode : public ASTNode {
public:
	CallASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::CALL;
	}

	Entity extractEntity() override;
};