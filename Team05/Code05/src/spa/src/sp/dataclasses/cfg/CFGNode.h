#pragma once

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;

class CFGNode {
protected:
	vector<shared_ptr<CFGNode>> nextNodes;
public:
	// Default value for statementNumber is -1 which is also invalid
	int statementNumber = -1;

	CFGNode() {}

	CFGNode(int statementNumber) {
		this->statementNumber = statementNumber;
	}

	virtual shared_ptr<CFGNode> getNext() {
		if (this->nextNodes.size() != 1) {
			throw CFGException("This node has more than one child node");
		}
		return this->nextNodes[0];
	}

	void addNext(shared_ptr<CFGNode> node) {
		this->nextNodes.push_back(node);
	}

	bool equals(shared_ptr<CFGNode> other) {
		if (this->nextNodes.size() != other->nextNodes.size()) {
			return false;
		}
		else {
			for (int i = 0; i < this->nextNodes.size(); i++) {
				// To prevent infinite recursion in the case of while
				if (this->nextNodes[i]->statementNumber < this->statementNumber) {
					// Recursively check child node
					if (!(this->nextNodes[i]->equals(other->nextNodes[i]))) {
						return false;
					}
				}
				// It is a recursive case where the child node is pointing to a parent while
				else {
					return this->nextNodes[i]->statementNumber == other->nextNodes[i]->statementNumber;
				}
			}
		}
		return this->statementNumber == other->statementNumber;
	}

	int hash() {
		return this->statementNumber;
	}

	virtual bool isIfNode() {
		return false;
	}

	virtual bool isWhileNode() {
		return false;
	}

	virtual bool hasNext() {
		return this->nextNodes.size() != 0;
	}
};
