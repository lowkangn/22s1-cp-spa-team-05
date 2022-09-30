#pragma once

#include <sp/dataclasses/ast/AST.h>

class ProgramASTNode : public ASTNode {
private:
	ProgramASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::PROGRAM;
	}
public:
	static shared_ptr<ASTNode> createProgramNode() {
		return shared_ptr<ASTNode> (new ProgramASTNode(Token::createProgramToken()));	
	}

	bool isTerminal() override {
		return false;
	}

	Entity extractEntity() override;
};