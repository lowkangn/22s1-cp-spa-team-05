#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>


class ReadASTNode : public ASTNode {
public:
	ReadASTNode (Token& token) : ASTNode(token) {
		this->type = ASTNodeType::READ;
	};

	shared_ptr<ASTNode> getVariableToRead() {
		return this->children[0];
	}

	Entity extractEntity() override;
};