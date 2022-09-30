#pragma once

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;

class WhileCFGNode : public CFGNode {
protected:
	WhileCFGNode(int statementNumber) : CFGNode(statementNumber) {

	}
public:
	static shared_ptr<WhileCFGNode> createWhileCFGNode(int statementNumber) {
		return shared_ptr<WhileCFGNode>(new WhileCFGNode(statementNumber));
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
		if (this->nextNodes.empty()) {
			throw CFGException("While node cannot have empty statement list");
		}
		return this->nextNodes[0];
	}

	virtual bool isWhileNode() override {
		return true;
	}

	virtual bool hasNext() override {
		return this->nextNodes.size() == 2;
	}
};
