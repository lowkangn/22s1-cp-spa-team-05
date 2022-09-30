#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class PrintASTNode : public ASTNode {
private:
	PrintASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PRINT;
	};
public:
	static shared_ptr<ASTNode> createPrintNode() {
		return shared_ptr<ASTNode>(new PrintASTNode(Token::createPrintToken()));
	}

	bool isTerminal() override {
		return false;
	}

	shared_ptr<ASTNode> getVariableToPrintNode() {
		// Ensure Print Node has only 1 children
		assert(this->getChildren().size() == 1);
		return this->children[0];
	}

	Entity extractEntity() override;
};