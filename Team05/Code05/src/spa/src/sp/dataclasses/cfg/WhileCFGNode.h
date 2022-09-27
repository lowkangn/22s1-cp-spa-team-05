#pragma once

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;

class WhileCFGNode : public CFGNode {

private:
	int statementNumber;
	shared_ptr<CFGNode> nextThenNode;
	shared_ptr<CFGNode> nextElseNode;
public:
	WhileCFGNode(int statementNumber) : statementNumber(statementNumber) {
		this->statementNumber = statementNumber;
	}

	int hash() {
		return this->statementNumber;
	}
};
