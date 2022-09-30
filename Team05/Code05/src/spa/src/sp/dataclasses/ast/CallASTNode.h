#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <assert.h>

class CallASTNode : public ASTNode {
private:
	CallASTNode(Token& token) : ASTNode(token) {
		this->type = ASTNodeType::CALL;
	}
public:
	static shared_ptr<ASTNode> createCallNode() {
		return shared_ptr<ASTNode>(new CallASTNode(Token::createCallToken()));
	}

	shared_ptr<ASTNode> getProcedureName() {
		// Should only have on child which is the procedure name ASTNode
		assert(this->getChildren().size() == 1);
		return this->children[0];
	}

	bool isTerminal() override {
		return false;
	}

	Entity extractEntity() override;
};