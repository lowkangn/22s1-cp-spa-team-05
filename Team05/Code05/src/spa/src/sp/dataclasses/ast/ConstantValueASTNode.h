#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>

// This type of ASTNode has no children
class ConstantValueASTNode : public ASTNode {
private:
	ConstantValueASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::CONSTANT;
	}
public:
	static shared_ptr<ASTNode> createConstantNode(Token token) {
		return shared_ptr<ASTNode>(new ConstantValueASTNode(token));
	}

	void addChild(shared_ptr<ASTNode> child) override;

	bool isTerminal() override;

	Entity extractEntity() override;
};