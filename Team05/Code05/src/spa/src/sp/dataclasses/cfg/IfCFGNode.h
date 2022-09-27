#pragma once

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;

class IfCFGNode : public CFGNode {

private:
	int statementNumber;
	shared_ptr<CFGNode> nextThenNode;
	shared_ptr<CFGNode> nextElseNode;
public:
	IfCFGNode(int statementNumber) : statementNumber(statementNumber) {
		this->statementNumber = statementNumber;
	}

	virtual shared_ptr<CFGNode> getNext() {
		throw CFGException("If CFG node has a then and else node!");
	}

	virtual void addNext(shared_ptr<CFGNode> node) {
		throw CFGException("If CFG node has a then and else node!");
	}

	void addNextThenNode(shared_ptr<CFGNode> node) {
		// Ensure that before adding the node is empty
		if (this->nextThenNode != nullptr) {
			throw CFGException("This node already has a nextThen node");
		}
		this->nextThenNode = node;
	}

	void addNextElseNode(shared_ptr<CFGNode> node) {
		// Ensure that before adding the node is empty
		if (this->nextElseNode != nullptr) {
			throw CFGException("This node already has a nextElse node");
		}
		this->nextElseNode = node;
	}

	shared_ptr<CFGNode> getThenNode() {
		if (this->nextThenNode == nullptr) {
			throw CFGException("This node does not have a nextThen node");
		}
		return this->nextThenNode;
	}

	shared_ptr<CFGNode> getElseNode() {
		if (this->nextElseNode == nullptr) {
			throw CFGException("This node does not have a nextElse node");
		}
		return this->nextElseNode;
	}

	int hash() {
		return this->statementNumber;
	}
};
