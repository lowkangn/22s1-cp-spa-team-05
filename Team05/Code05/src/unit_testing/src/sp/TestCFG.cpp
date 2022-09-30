#include "catch.hpp"
#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>
#include <vector>

using namespace std;

TEST_CASE("CFG: Add Child") {
	auto testAddChild = [](shared_ptr<CFGNode> nodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addChild(nodeToAdd);

		REQUIRE(nodeToAddTo->getNext()->equals(nodeToAdd));
	};

	shared_ptr<CFGNode> nodeToAdd = CFGNode::createCFGNode(1);
	shared_ptr<CFGNode> nodeToAddTo = CFGNode::createCFGNode(2);

	testAddChild(nodeToAdd, nodeToAddTo);
};

TEST_CASE("CFG: add child to IfCFGNode") {
	auto testAddChildToIfCFG = [](shared_ptr<CFGNode> thenNodeToAdd, shared_ptr<CFGNode> elseNodeToAdd, shared_ptr<IfCFGNode> nodeToAddTo) {
		nodeToAddTo->addChild(thenNodeToAdd);
		nodeToAddTo->addChild(elseNodeToAdd);

		shared_ptr<IfCFGNode> nodeToCheck = dynamic_pointer_cast<IfCFGNode>(nodeToAddTo);

		REQUIRE(nodeToCheck->getThenNode()->equals(thenNodeToAdd));
		REQUIRE(nodeToCheck->getElseNode()->equals(elseNodeToAdd));
	};

	auto testAddChildToIfCFGThrows = [](shared_ptr<CFGNode> thenNodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addChild(thenNodeToAdd);

		shared_ptr<IfCFGNode> nodeToCheck = dynamic_pointer_cast<IfCFGNode>(nodeToAddTo);

		REQUIRE_THROWS(nodeToCheck->getThenNode() == thenNodeToAdd);
	};

	shared_ptr<CFGNode> thenNodeToAdd = CFGNode::createCFGNode(4);
	shared_ptr<CFGNode> elseNodeToAdd = CFGNode::createCFGNode(3);
	shared_ptr<IfCFGNode> nodeToAddTo = IfCFGNode::createIfCFGNode(2);

	shared_ptr<CFGNode> thenNodeToAdd2 = CFGNode::createCFGNode(4);
	shared_ptr<CFGNode> nodeToAddTo2 = IfCFGNode::createIfCFGNode(2);

	SECTION("Test adding child to CFG") {
		testAddChildToIfCFG(thenNodeToAdd, elseNodeToAdd, nodeToAddTo);
	}

	SECTION("Test throws adding child to CFG") {
		testAddChildToIfCFGThrows(thenNodeToAdd2, nodeToAddTo2);
	}
};


TEST_CASE("CFG: add child to WhileCFGNode") {
	auto testAddChildToEndOfWhileCFG = [](shared_ptr<CFGNode> nextNodeToAdd, shared_ptr<CFGNode> loopNodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addChild(loopNodeToAdd);
		nodeToAddTo->addChild(nextNodeToAdd);

		shared_ptr<WhileCFGNode> nodeToCheck = dynamic_pointer_cast<WhileCFGNode>(nodeToAddTo);

		REQUIRE(nodeToCheck->getAfterWhile()->equals(nextNodeToAdd));
		REQUIRE(nodeToCheck->getInLoop()->equals(loopNodeToAdd));
	};

	auto testAddChildToEndOfWhileCFGThrows = [](shared_ptr<CFGNode> nextNodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addChild(nextNodeToAdd);

		shared_ptr<WhileCFGNode> nodeToCheck = dynamic_pointer_cast<WhileCFGNode>(nodeToAddTo);

		REQUIRE_THROWS(nodeToCheck->getNext() == nextNodeToAdd);
	};

	shared_ptr<CFGNode> nextNodeToAdd = CFGNode::createCFGNode(4);
	shared_ptr<CFGNode> loopNodeToAdd = CFGNode::createCFGNode(3);
	shared_ptr<CFGNode> nodeToAddTo = WhileCFGNode::createWhileCFGNode(2);

	shared_ptr<CFGNode> nextNodeToAdd2 = CFGNode::createCFGNode(4);
	shared_ptr<CFGNode> nodeToAddTo2 = WhileCFGNode::createWhileCFGNode(2);

	testAddChildToEndOfWhileCFG(nextNodeToAdd, loopNodeToAdd, nodeToAddTo);
	testAddChildToEndOfWhileCFGThrows(nextNodeToAdd2, nodeToAddTo2);
};

TEST_CASE("CFG: test equals") {
	auto testEquals = [](shared_ptr<CFGNode> nodeOne, shared_ptr<CFGNode> nodeTwo) {
		REQUIRE(nodeOne->equals(nodeTwo));
	};

	shared_ptr<CFGNode> nodeOne = CFGNode::createCFGNode(4);
	shared_ptr<CFGNode> nodeOneCopy = CFGNode::createCFGNode(4);

	shared_ptr<CFGNode> nodeTwo = CFGNode::createCFGNode(4);
	shared_ptr<CFGNode> nodeChild = CFGNode::createCFGNode(5);
	nodeTwo->addChild(nodeChild);

	shared_ptr<CFGNode> nodeTwoCopy = CFGNode::createCFGNode(4);
	shared_ptr<CFGNode> nodeChildCopy = CFGNode::createCFGNode(5);
	nodeTwoCopy->addChild(nodeChildCopy);

	testEquals(nodeOne, nodeOneCopy);
	testEquals(nodeTwo, nodeTwoCopy);
};
