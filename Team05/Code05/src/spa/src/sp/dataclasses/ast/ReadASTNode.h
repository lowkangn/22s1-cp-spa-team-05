#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>

#include <memory>

class ReadASTNode : public ASTNode {
private:
    explicit ReadASTNode(Token& token) : ASTNode(token) {
        this->type = ASTNodeType::READ;
    }

public:
    static shared_ptr<ASTNode> createReadNode() {
        return shared_ptr<ASTNode>(new ReadASTNode(Token::createReadToken()));
    }

    bool isTerminal() override {
        return false;
    }

    shared_ptr<ASTNode> getVariableToRead() {
        // Ensure Read Node has only 1 children
        assert(this->getChildren().size() == 1);
        // Ensure Read Node only has variable node as child
        assert(this->children[0]->getType() == ASTNodeType::VARIABLE);
        return this->children[0];
    }

    Entity extractEntity() override;
};
