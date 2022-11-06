#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>

#include <memory>

// This type of ASTNode has no children
class VariableASTNode : public ASTNode {
private:
    explicit VariableASTNode(Token& token) : ASTNode(token) {
        this->type = ASTNodeType::VARIABLE;
    }

public:
    static shared_ptr<ASTNode> createVariableNode(Token token) {
        return shared_ptr<ASTNode>(new VariableASTNode(token));
    }

    void addChild(shared_ptr<ASTNode> child) override {
        throw ASTException("Cannot add child to variable node");
    }

    bool isTerminal() override {
        return true;
    }

    Entity extractEntity() override;
};
