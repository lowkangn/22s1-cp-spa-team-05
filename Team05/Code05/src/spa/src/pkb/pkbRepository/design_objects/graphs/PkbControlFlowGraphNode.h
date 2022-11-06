#pragma once

#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbGraphNode.h>

#include <memory>
#include <string>

using namespace std;

class PkbControlFlowGraphNode : public PkbGraphNode {
private:
    shared_ptr<PkbStatementEntity> statementEntity; // explicit statement entity being wrapped

public:
    explicit PkbControlFlowGraphNode(shared_ptr<PkbStatementEntity> statementEntity)
        : PkbGraphNode(shared_ptr<PkbEntity>(statementEntity)) {
        this->statementEntity = statementEntity;
    }

    // =========== Factories ====================

    static shared_ptr<PkbGraphNode> createPkbControlFlowGraphNode(
        shared_ptr<PkbStatementEntity> statementEntity) {
        return make_shared<PkbControlFlowGraphNode>(statementEntity);
    }

    // ========== identifier methods ============

    bool isReadStatementNode() {
        return this->statementEntity->isReadStatement();
    }

    bool isPrintStatementNode() {
        return this->statementEntity->isPrintStatement();
    }

    bool isWhileStatementNode() {
        return this->statementEntity->isWhileStatement();
    }

    bool isAssignStatementNode() {
        return this->statementEntity->isAssignStatement();
    }

    bool isIfStatementNode() {
        return this->statementEntity->isIfStatement();
    }

    bool isCallStatementNode() {
        return this->statementEntity->isCallStatement();
    }

    int getStatementLineNumber() {
        return this->entity->getLineNumber();
    }

    string getKey() override {
        return this->statementEntity->getKey();
    }
};
