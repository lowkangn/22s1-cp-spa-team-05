#include "catch.hpp"
#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>
#include <vector>

using namespace std;

TEST_CASE("CFG: Add Child") {
	auto testAddChild = [](shared_ptr<CFGNode> nodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addNext(nodeToAdd);

		REQUIRE(nodeToAddTo->getNext()->equals(nodeToAdd));
	};

	shared_ptr<CFGNode> nodeToAdd(new CFGNode(1));
	shared_ptr<CFGNode> nodeToAddTo(new CFGNode(2));

	testAddChild(nodeToAdd, nodeToAddTo);
};

TEST_CASE("CFG: add child to IfCFGNode") {
	auto testAddChildToIfCFG = [](shared_ptr<CFGNode> thenNodeToAdd, shared_ptr<CFGNode> elseNodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addNext(thenNodeToAdd);
		nodeToAddTo->addNext(elseNodeToAdd);

		shared_ptr<IfCFGNode> nodeToCheck = dynamic_pointer_cast<IfCFGNode>(nodeToAddTo);

		REQUIRE(nodeToCheck->getThenNode()->equals(thenNodeToAdd));
		REQUIRE(nodeToCheck->getElseNode()->equals(elseNodeToAdd));
	};

	auto testAddChildToIfCFGThrows = [](shared_ptr<CFGNode> thenNodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addNext(thenNodeToAdd);

		shared_ptr<IfCFGNode> nodeToCheck = dynamic_pointer_cast<IfCFGNode>(nodeToAddTo);

		REQUIRE_THROWS(nodeToCheck->getThenNode() == thenNodeToAdd);
	};

	shared_ptr<CFGNode> thenNodeToAdd(new CFGNode(4));
	shared_ptr<CFGNode> elseNodeToAdd(new CFGNode(3));
	shared_ptr<CFGNode> nodeToAddTo(new IfCFGNode(2));

	shared_ptr<CFGNode> thenNodeToAdd2(new CFGNode(4));
	shared_ptr<CFGNode> nodeToAddTo2(new IfCFGNode(2));

	testAddChildToIfCFG(thenNodeToAdd, elseNodeToAdd, nodeToAddTo);
	testAddChildToIfCFGThrows(thenNodeToAdd2, nodeToAddTo2);
};


TEST_CASE("CFG: add child to WhileCFGNode") {
	auto testAddChildToEndOfWhileCFG = [](shared_ptr<CFGNode> nextNodeToAdd, shared_ptr<CFGNode> loopNodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addNext(loopNodeToAdd);
		nodeToAddTo->addNext(nextNodeToAdd);

		shared_ptr<WhileCFGNode> nodeToCheck = dynamic_pointer_cast<WhileCFGNode>(nodeToAddTo);

		REQUIRE(nodeToCheck->getAfterWhile()->equals(nextNodeToAdd));
		REQUIRE(nodeToCheck->getInLoop()->equals(loopNodeToAdd));
	};

	auto testAddChildToEndOfWhileCFGThrows = [](shared_ptr<CFGNode> nextNodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addNext(nextNodeToAdd);

		shared_ptr<WhileCFGNode> nodeToCheck = dynamic_pointer_cast<WhileCFGNode>(nodeToAddTo);

		REQUIRE_THROWS(nodeToCheck->getNext() == nextNodeToAdd);
	};

	shared_ptr<CFGNode> nextNodeToAdd(new CFGNode(4));
	shared_ptr<CFGNode> loopNodeToAdd(new CFGNode(3));
	shared_ptr<CFGNode> nodeToAddTo(new WhileCFGNode(2));

	shared_ptr<CFGNode> nextNodeToAdd2(new CFGNode(4));
	shared_ptr<CFGNode> nodeToAddTo2(new WhileCFGNode(2));

	testAddChildToEndOfWhileCFG(nextNodeToAdd, loopNodeToAdd, nodeToAddTo);
	testAddChildToEndOfWhileCFGThrows(nextNodeToAdd2, nodeToAddTo2);
};

TEST_CASE("CFG: test equals") {
	auto testEquals = [](shared_ptr<CFGNode> nodeOne, shared_ptr<CFGNode> nodeTwo) {
		REQUIRE(nodeOne->equals(nodeTwo));
	};

	shared_ptr<CFGNode> nodeOne(new CFGNode(4));
	shared_ptr<CFGNode> nodeOneCopy(new CFGNode(4));

	shared_ptr<CFGNode> nodeTwo(new CFGNode(4));
	shared_ptr<CFGNode> nodeChild(new CFGNode(5));
	nodeTwo->addNext(nodeChild);

	shared_ptr<CFGNode> nodeTwoCopy(new CFGNode(4));
	shared_ptr<CFGNode> nodeChildCopy(new CFGNode(5));
	nodeTwoCopy->addNext(nodeChildCopy);

	testEquals(nodeOne, nodeOneCopy);
	testEquals(nodeTwo, nodeTwoCopy);
};
