#pragma once

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;

class EndOfWhileCFGNode : public CFGNode {

public:
	EndOfWhileCFGNode(int statementNumber) : CFGNode(statementNumber) {
	
	}

	virtual shared_ptr<CFGNode> getNext() override {
		if (this->nextNodes.size() != 2) {
			throw CFGException("EndOfWhileNode is supposed to have 2 children");
		}
		else {
			return this->nextNodes[0];
		}
	}

	shared_ptr<CFGNode> getLoopNode() {
		if (this->nextNodes.size() != 2) {
			throw CFGException("EndOfWhileNode is supposed to have 2 children");
		}
		else {
			return this->nextNodes[1];
		}
	}
};
