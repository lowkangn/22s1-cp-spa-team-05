#pragma once

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;

class WhileCFGNode : public CFGNode {
public:
	WhileCFGNode(int statementNumber) : CFGNode(statementNumber) {

	}

	virtual shared_ptr<CFGNode> getNext() override {
		throw CFGException("While CFG node has two child nodes");
	}

	shared_ptr<CFGNode> getAfterWhile() {
		if (this->nextNodes.size() != 2) {
			throw CFGException("While node is supposed to have 2 children");
		}
		else {
			return this->nextNodes[1];
		}
	}

	shared_ptr<CFGNode> getInLoop() {
		if (this->nextNodes.size() != 2) {
			throw CFGException("While node is supposed to have 2 children");
		}
		else {
			return this->nextNodes[0];
		}
	}
};
