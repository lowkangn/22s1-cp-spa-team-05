#pragma once

#include <assert.h>

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/exceptions/CFGException.h>

#include <memory>
#include <vector>
using namespace std;

class IfCFGNode : public CFGNode {
protected:
    explicit IfCFGNode(Entity entity) : CFGNode(entity) {
    }

public:
    static shared_ptr<IfCFGNode> createIfCFGNode(int statementNumber) {
        return shared_ptr<IfCFGNode>(new IfCFGNode(Entity::createIfEntity(statementNumber)));
    }

    shared_ptr<CFGNode> getNext() override {
        throw CFGException("If CFG node has to have child nodes");
    }

    shared_ptr<CFGNode> getThenNode() {
        if (this->nextNodes.size() != 2) {
            throw CFGException("If node is supposed to have 2 children");
        } else {
            return this->nextNodes[0];
        }
    }

    shared_ptr<CFGNode> getElseNode() {
        if (this->nextNodes.size() != 2) {
            throw CFGException("If node is supposed to have 2 children");
        } else {
            return this->nextNodes[1];
        }
    }

    bool isIfNode() override {
        return true;
    }

    bool hasNext() override {
        shared_ptr<CFGNode> thenCFGNode = this->getThenNode();
        shared_ptr<CFGNode> elseCFGNode = this->getElseNode();

        if (!thenCFGNode->hasNext() && !elseCFGNode->hasNext()) {
            return false;
        }

        return true;
    }
};
