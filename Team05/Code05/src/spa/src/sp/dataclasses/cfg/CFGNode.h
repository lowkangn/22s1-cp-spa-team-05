#pragma once

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;

class CFGNode {

private:
	int statementNumber;
	shared_ptr<CFGNode> nextNode;
public:
	CFGNode(int statementNumber) : statementNumber(statementNumber) {
		this->statementNumber = statementNumber;
	}

	virtual shared_ptr<CFGNode> getNext() {
		if (this->nextNode == nullptr) {
			throw CFGException("This node does not have a next node");
		}
		return this->nextNode;
	}

	virtual void addNext(shared_ptr<CFGNode> node) {
		// Ensure that before adding the node is empty
		if (this->nextNode != nullptr) {
			throw CFGException("This node already has a next node");
		}
		this->nextNode = node;
	}

	int hash() {
		return this->statementNumber;
	}

};
