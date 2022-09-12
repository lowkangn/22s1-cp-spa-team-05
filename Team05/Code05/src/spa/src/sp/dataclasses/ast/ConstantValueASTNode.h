#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>

// This type of ASTNode has no children
class ConstantValueASTNode : public ASTNode {
public:
	ConstantValueASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::CONSTANT;
	}

	void addChild(shared_ptr<ASTNode> child) override;

	bool isTerminal() override;

	Entity extractEntity() override;
};