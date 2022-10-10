#pragma once

#include <pkb/design_objects/graphs/PkbGraphNode.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>


#include <memory>
#include <vector>
#include <string>
using namespace std;

class PkbControlFlowGraphNode : public PkbGraphNode {

private: 
	shared_ptr<PkbStatementEntity> statementEntity; // explicit statement entity being wrapped

protected:
	PkbControlFlowGraphNode(shared_ptr<PkbStatementEntity> statementEntity) : PkbGraphNode(new shared_ptr<PkbEntity>(statementEntity)) {
		this->statementEntity = statementEntity;
	}

public:

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

    
    string getKey() override {
        return this->statementEntity->getKey();
    }
	
};