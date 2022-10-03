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

	auto testNotEquals = [](shared_ptr<CFGNode> nodeOne, shared_ptr<CFGNode> nodeTwo) {
		REQUIRE(!nodeOne->equals(nodeTwo));
	};

	SECTION("Basic test") {
		shared_ptr<CFGNode> nodeOne = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> nodeOneCopy = CFGNode::createCFGNode(4);

		testEquals(nodeOne, nodeOneCopy);
	}

	SECTION("Test with no cycle and one child") {
		shared_ptr<CFGNode> nodeTwo = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> nodeChild = CFGNode::createCFGNode(5);
		nodeTwo->addChild(nodeChild);

		shared_ptr<CFGNode> nodeTwoCopy = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> nodeChildCopy = CFGNode::createCFGNode(5);
		nodeTwoCopy->addChild(nodeChildCopy);

		testEquals(nodeTwo, nodeTwoCopy);
	}

	SECTION("Equals: Test with if") {
		shared_ptr<IfCFGNode> ifNode = IfCFGNode::createIfCFGNode(1);

		shared_ptr<CFGNode> thenStatementOne = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> thenStatementTwo = CFGNode::createCFGNode(3);

		shared_ptr<CFGNode> elseStatementOne = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> elseStatementTwo = CFGNode::createCFGNode(5);

		shared_ptr<CFGNode> outsideIf = CFGNode::createCFGNode(6);

		shared_ptr<CFGNode> ifNodeCopy = IfCFGNode::createIfCFGNode(1);

		shared_ptr<CFGNode> thenStatementOneCopy = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> thenStatementTwoCopy = CFGNode::createCFGNode(3);

		shared_ptr<CFGNode> elseStatementOneCopy = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> elseStatementTwoCopy = CFGNode::createCFGNode(5);

		shared_ptr<CFGNode> outsideIfCopy = CFGNode::createCFGNode(6);

		ifNode->addChild(thenStatementOne);
		ifNode->addChild(elseStatementOne);

		thenStatementOne->addChild(thenStatementTwo);
		elseStatementOne->addChild(elseStatementTwo);

		thenStatementTwo->addChild(outsideIf);
		elseStatementTwo->addChild(outsideIf);

		ifNodeCopy->addChild(thenStatementOneCopy);
		ifNodeCopy->addChild(elseStatementOneCopy);

		thenStatementOneCopy->addChild(thenStatementTwoCopy);
		elseStatementOneCopy->addChild(elseStatementTwoCopy);

		thenStatementTwoCopy->addChild(outsideIfCopy);
		elseStatementTwoCopy->addChild(outsideIfCopy);

		testEquals(ifNode, ifNodeCopy);
	}

	SECTION("Not Equals: Test with if") {
		shared_ptr<IfCFGNode> ifNode = IfCFGNode::createIfCFGNode(1);

		shared_ptr<CFGNode> thenStatementOne = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> thenStatementTwo = CFGNode::createCFGNode(3);

		shared_ptr<CFGNode> elseStatementOne = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> elseStatementTwo = CFGNode::createCFGNode(5);

		shared_ptr<CFGNode> outsideIf = CFGNode::createCFGNode(6);
		shared_ptr<CFGNode> outsideIfDiff = CFGNode::createCFGNode(7);

		shared_ptr<CFGNode> ifNodeCopy = IfCFGNode::createIfCFGNode(1);

		shared_ptr<CFGNode> thenStatementOneCopy = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> thenStatementTwoCopy = CFGNode::createCFGNode(3);

		shared_ptr<CFGNode> elseStatementOneCopy = CFGNode::createCFGNode(4);
		shared_ptr<CFGNode> elseStatementTwoCopy = CFGNode::createCFGNode(5);

		shared_ptr<CFGNode> outsideIfCopy = CFGNode::createCFGNode(6);

		// test if then and else point to diff statements after
		ifNode->addChild(thenStatementOne);
		ifNode->addChild(elseStatementOne);

		thenStatementOne->addChild(thenStatementTwo);
		elseStatementOne->addChild(elseStatementTwo);

		thenStatementTwo->addChild(outsideIf);
		elseStatementTwo->addChild(outsideIfDiff);

		ifNodeCopy->addChild(thenStatementOneCopy);
		ifNodeCopy->addChild(elseStatementOneCopy);

		thenStatementOneCopy->addChild(thenStatementTwoCopy);
		elseStatementOneCopy->addChild(elseStatementTwoCopy);

		thenStatementTwoCopy->addChild(outsideIfCopy);
		elseStatementTwoCopy->addChild(outsideIfCopy);

		testNotEquals(ifNode, ifNodeCopy);
	}

	SECTION("Equals: Test with while") {
		shared_ptr<WhileCFGNode> whileNode = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> inWhile = CFGNode::createCFGNode(2);

		shared_ptr<CFGNode> afterWhile = CFGNode::createCFGNode(3);

		shared_ptr<WhileCFGNode> whileNodeCopy = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> inWhileCopy = CFGNode::createCFGNode(2);

		shared_ptr<CFGNode> afterWhileCopy = CFGNode::createCFGNode(3);

		whileNode->addChild(inWhile);
		inWhile->addChild(whileNode);
		whileNode->addChild(afterWhile);

		whileNodeCopy->addChild(inWhileCopy);
		inWhileCopy->addChild(whileNodeCopy);
		whileNodeCopy->addChild(afterWhileCopy);

		testEquals(whileNode, whileNodeCopy);
	}

	SECTION("Not Equals: Test with while, different whiles") {
		shared_ptr<WhileCFGNode> whileNode = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> inWhile = CFGNode::createCFGNode(2);

		shared_ptr<CFGNode> afterWhile = CFGNode::createCFGNode(3);

		shared_ptr<WhileCFGNode> whileNodeCopy = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> inWhileCopyOne = CFGNode::createCFGNode(2);
		shared_ptr<CFGNode> inWhileCopyTwo = CFGNode::createCFGNode(3);

		shared_ptr<CFGNode> afterWhileCopy = CFGNode::createCFGNode(4);

		whileNode->addChild(inWhile);
		inWhile->addChild(whileNode);
		whileNode->addChild(afterWhile);

		whileNodeCopy->addChild(inWhileCopyOne);
		inWhileCopyOne->addChild(inWhileCopyTwo);
		inWhileCopyTwo->addChild(whileNodeCopy);
		whileNodeCopy->addChild(afterWhileCopy);

		testNotEquals(whileNode, whileNodeCopy);
	}

	SECTION("Not Equals: Test with while, different nextNode") {
		shared_ptr<WhileCFGNode> whileNode = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> inWhile = CFGNode::createCFGNode(2);

		shared_ptr<CFGNode> afterWhile = CFGNode::createCFGNode(3);

		shared_ptr<WhileCFGNode> whileNodeCopy = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> inWhileCopy = CFGNode::createCFGNode(2);

		shared_ptr<CFGNode> afterWhileCopyOne = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> afterWhileCopyTwo = CFGNode::createCFGNode(3);

		whileNode->addChild(inWhile);
		inWhile->addChild(whileNode);
		whileNode->addChild(afterWhile);

		whileNodeCopy->addChild(inWhileCopy);
		inWhileCopy->addChild(whileNodeCopy);
		whileNodeCopy->addChild(afterWhileCopyOne);
		afterWhileCopyOne->addChild(afterWhileCopyTwo);

		testNotEquals(whileNode, whileNodeCopy);
	}

	SECTION("Equals: Test with if inside while") {
		shared_ptr<WhileCFGNode> whileNode = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> ifInWhile = IfCFGNode::createCFGNode(2);

		shared_ptr<CFGNode> thenCFG = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> inThenCFG = CFGNode::createCFGNode(4);

		shared_ptr<CFGNode> elseCFG = CFGNode::createCFGNode(5);
		shared_ptr<CFGNode> inElseCFG = CFGNode::createCFGNode(6);

		shared_ptr<CFGNode> outWhile = CFGNode::createCFGNode(7);

		shared_ptr<WhileCFGNode> whileNodeCopy = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> ifInWhileCopy = IfCFGNode::createCFGNode(2);

		shared_ptr<CFGNode> thenCFGCopy = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> inThenCFGCopy = CFGNode::createCFGNode(4);

		shared_ptr<CFGNode> elseCFGCopy = CFGNode::createCFGNode(5);
		shared_ptr<CFGNode> inElseCFGCopy = CFGNode::createCFGNode(6);

		shared_ptr<CFGNode> outWhileCopy = CFGNode::createCFGNode(7);


		whileNode->addChild(ifInWhile);
		ifInWhile->addChild(thenCFG);
		ifInWhile->addChild(elseCFG);

		thenCFG->addChild(inThenCFG);
		elseCFG->addChild(inElseCFG);

		inThenCFG->addChild(whileNode);
		inElseCFG->addChild(whileNode);

		whileNode->addChild(outWhile);

		whileNodeCopy->addChild(ifInWhileCopy);
		ifInWhileCopy->addChild(thenCFGCopy);
		ifInWhileCopy->addChild(elseCFGCopy);

		thenCFGCopy->addChild(inThenCFGCopy);
		elseCFGCopy->addChild(inElseCFGCopy);

		inThenCFGCopy->addChild(whileNodeCopy);
		inElseCFGCopy->addChild(whileNodeCopy);

		whileNodeCopy->addChild(outWhileCopy);

		testEquals(whileNode, whileNodeCopy);
	}

	SECTION("Not Equals: Test with if inside while") {
		shared_ptr<WhileCFGNode> whileNode = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> ifInWhile = IfCFGNode::createCFGNode(2);

		shared_ptr<CFGNode> thenCFG = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> inThenCFG = CFGNode::createCFGNode(4);

		shared_ptr<CFGNode> elseCFG = CFGNode::createCFGNode(5);
		// Else has an extra statement
		shared_ptr<CFGNode> inElseCFGOne = CFGNode::createCFGNode(6);
		shared_ptr<CFGNode> inElseCFGTwo = CFGNode::createCFGNode(7);

		shared_ptr<CFGNode> outWhile = CFGNode::createCFGNode(8);

		shared_ptr<WhileCFGNode> whileNodeCopy = WhileCFGNode::createWhileCFGNode(1);

		shared_ptr<CFGNode> ifInWhileCopy = IfCFGNode::createCFGNode(2);

		shared_ptr<CFGNode> thenCFGCopy = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> inThenCFGCopy = CFGNode::createCFGNode(4);

		shared_ptr<CFGNode> elseCFGCopy = CFGNode::createCFGNode(5);
		shared_ptr<CFGNode> inElseCFGCopy = CFGNode::createCFGNode(6);

		shared_ptr<CFGNode> outWhileCopy = CFGNode::createCFGNode(7);


		whileNode->addChild(ifInWhile);
		ifInWhile->addChild(thenCFG);
		ifInWhile->addChild(elseCFG);

		thenCFG->addChild(inThenCFG);
		elseCFG->addChild(inElseCFGOne);
		inElseCFGOne->addChild(inElseCFGTwo);

		inThenCFG->addChild(whileNode);
		inElseCFGTwo->addChild(whileNode);

		whileNode->addChild(outWhile);

		whileNodeCopy->addChild(ifInWhileCopy);
		ifInWhileCopy->addChild(thenCFGCopy);
		ifInWhileCopy->addChild(elseCFGCopy);

		thenCFGCopy->addChild(inThenCFGCopy);
		elseCFGCopy->addChild(inElseCFGCopy);

		inThenCFGCopy->addChild(whileNodeCopy);
		inElseCFGCopy->addChild(whileNodeCopy);

		whileNodeCopy->addChild(outWhileCopy);

		testNotEquals(whileNode, whileNodeCopy);
	}

	SECTION("Not Equals: Test with nested while") {
		shared_ptr<WhileCFGNode> whileNode = WhileCFGNode::createWhileCFGNode(1);
		shared_ptr<WhileCFGNode> inWhile = WhileCFGNode::createWhileCFGNode(2);
		shared_ptr<CFGNode> inNestedWhile = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> outWhile = CFGNode::createCFGNode(4);

		shared_ptr<WhileCFGNode> whileNodeCopy = WhileCFGNode::createWhileCFGNode(1);
		shared_ptr<WhileCFGNode> inWhileCopy = WhileCFGNode::createWhileCFGNode(2);
		shared_ptr<CFGNode> inNestedWhileCopy = CFGNode::createCFGNode(3);
		shared_ptr<CFGNode> outWhileCopy = CFGNode::createCFGNode(4);


		whileNode->addChild(inWhile);
		inWhile->addChild(inNestedWhile);
		inWhile->addChild(whileNode);
		whileNode->addChild(outWhile);

		whileNodeCopy->addChild(inWhileCopy);
		inWhileCopy->addChild(inNestedWhileCopy);
		inWhileCopy->addChild(whileNodeCopy);
		whileNodeCopy->addChild(outWhileCopy);

		testEquals(whileNode, whileNodeCopy);
	}
};
