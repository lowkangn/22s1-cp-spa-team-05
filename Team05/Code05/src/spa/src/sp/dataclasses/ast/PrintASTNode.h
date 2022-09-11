#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class PrintASTNode : public ASTNode {
public:
	PrintASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PRINT;
	};

	bool isTerminal() {
		return false;
	}

	shared_ptr<ASTNode> getVariableToPrintNode() {
		return this->children[0];
	}

	Entity extractEntity() override;
};