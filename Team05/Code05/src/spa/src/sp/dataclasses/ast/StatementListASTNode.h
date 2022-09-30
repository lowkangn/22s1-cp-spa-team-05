#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>

class StatementListASTNode : public ASTNode {
private:
	StatementListASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::STMTLIST;
	}
public:
	static shared_ptr<ASTNode> createStatementListNode() {
		return shared_ptr<ASTNode>(new StatementListASTNode(Token::createPlaceholderToken())); 
	}

	bool isTerminal() override {
		return false;
	}

	Entity extractEntity() override;
};