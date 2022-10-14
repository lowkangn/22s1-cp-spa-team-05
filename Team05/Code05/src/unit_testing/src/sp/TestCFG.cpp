#include "catch.hpp"
#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>
#include <vector>
#include <unordered_map>

using namespace std;

TEST_CASE("CFG: Add Child") {
	auto testAddChild = [](shared_ptr<CFGNode> nodeToAdd, shared_ptr<CFGNode> nodeToAddTo) {
		nodeToAddTo->addChild(nodeToAdd);

		REQUIRE(nodeToAddTo->getNext()->equals(nodeToAdd));
	};

	shared_ptr<CFGNode> nodeToAdd = CFGNode::createCFGNode(Entity::createAssignEntity(1));
	shared_ptr<CFGNode> nodeToAddTo = CFGNode::createCFGNode(Entity::createAssignEntity(2));

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

	shared_ptr<CFGNode> thenNodeToAdd = CFGNode::createCFGNode(Entity::createAssignEntity(4));
	shared_ptr<CFGNode> elseNodeToAdd = CFGNode::createCFGNode(Entity::createAssignEntity(3));
	shared_ptr<IfCFGNode> nodeToAddTo = IfCFGNode::createIfCFGNode(2);

	shared_ptr<CFGNode> thenNodeToAdd2 = CFGNode::createCFGNode(Entity::createAssignEntity(3));
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

	shared_ptr<CFGNode> nextNodeToAdd = CFGNode::createCFGNode(Entity::createAssignEntity(4));
	shared_ptr<CFGNode> loopNodeToAdd = CFGNode::createCFGNode(Entity::createAssignEntity(3));
	shared_ptr<CFGNode> nodeToAddTo = WhileCFGNode::createWhileCFGNode(2);

	shared_ptr<CFGNode> nextNodeToAdd2 = CFGNode::createCFGNode(Entity::createAssignEntity(4));
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


		shared_ptr<CFGNode> nodeOne = CFGNode::createCFGNode(Entity::createAssignEntity(4));
		shared_ptr<CFGNode> nodeOneCopy = CFGNode::createCFGNode(Entity::createAssignEntity(4));

		testEquals(nodeOne, nodeOneCopy);
	}

	SECTION("Test with no cycle and one child") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))}
		};

		unordered_map<int, vector<int>> adjList = {
			{4, {5}}
		};

		shared_ptr<CFGNode> nodeTwo = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 4);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))}
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{4, {5}}
		};

		shared_ptr<CFGNode> nodeTwoCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMapCopy, adjListCopy, 4);

		testEquals(nodeTwo, nodeTwoCopy);
	}

	SECTION("Equals: Test with if") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, IfCFGNode::createIfCFGNode(1)},
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 3, 5}},
			{2, {4}},
			{3, {5}},
		};

		shared_ptr<CFGNode> ifNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, IfCFGNode::createIfCFGNode(1)},
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 3, 5}},
			{2, {4}},
			{3, {5}},
		};

		shared_ptr<CFGNode> ifNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		testEquals(ifNode, ifNodeCopy);
	}

	SECTION("Not Equals: Test with if") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, IfCFGNode::createIfCFGNode(1)},
			// Then
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			// Else
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			// Then statement
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Else statement
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			// Outside if
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 3, 6}},
			{2, {4}},
			{3, {5}},
			{6, {7}},
		};

		shared_ptr<CFGNode> ifNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, IfCFGNode::createIfCFGNode(1)},
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 3, 5}},
			{2, {4}},
			{3, {5}},
		};

		shared_ptr<CFGNode> ifNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMapCopy, adjListCopy, 1);

		testNotEquals(ifNode, ifNodeCopy);
	}

	SECTION("Equals: Test with while") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// In while
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			// Outside while
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 3}},
			{2, {1}},
		};

		shared_ptr<CFGNode> whileNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// In while
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			// Outside while
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 3}},
			{2, {1}},
		};

		shared_ptr<CFGNode> whileNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMapCopy, adjListCopy, 1);

		testEquals(whileNode, whileNodeCopy);
	}

	SECTION("Not Equals: Test with while, different whiles") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// In while
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			// Outside while
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 3}},
			{2, {1}},
		};

		shared_ptr<CFGNode> whileNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// In while
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			// Outside while
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 4}},
			{2, {3}},
			{3, {1}},
		};

		shared_ptr<CFGNode> whileNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMapCopy, adjListCopy, 1);

		testNotEquals(whileNode, whileNodeCopy);
	}

	SECTION("Not Equals: Test with while, different nextNode") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// In while
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			// Outside while
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 3}},
			{2, {1}},
		};

		shared_ptr<CFGNode> whileNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// In while
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			// Outside while
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 3}},
			{2, {1}},
			{3, {4}},
		};

		shared_ptr<CFGNode> whileNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMapCopy, adjListCopy, 1);

		testNotEquals(whileNode, whileNodeCopy);
	}

	SECTION("Equals: Test with if inside while") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// If In while
			{2, IfCFGNode::createIfCFGNode(2)},
			// Then
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Else
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			// Outside while
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 7}},
			{2, {3,5}},
			{3, {4}},
			{5, {6}},
		};

		shared_ptr<CFGNode> whileNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// If In while
			{2, IfCFGNode::createIfCFGNode(2)},
			// Then
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Else
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			// Outside while
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 7}},
			{2, {3,5}},
			{3, {4}},
			{5, {6}},
		};

		shared_ptr<CFGNode> whileNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMapCopy, adjListCopy, 1);

		testEquals(whileNode, whileNodeCopy);
	}

	SECTION("Not Equals: Test with if inside while") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// If In while
			{2, IfCFGNode::createIfCFGNode(2)},
			// Then
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Else
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			// Outside if statement
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
			// Outside while
			{8, CFGNode::createCFGNode(Entity::createAssignEntity(8))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 8}},
			{2, {3,5}},
			{3, {4}},
			{5, {6}},
			{4, {7}},
			{6, {7}},
			{7, {1}},
		};

		shared_ptr<CFGNode> whileNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// If In while
			{2, IfCFGNode::createIfCFGNode(2)},
			// Then
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Else
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			// Outside while
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 7}},
			{2, {3,5}},
			{3, {4}},
			{5, {6}},
		};

		shared_ptr<CFGNode> whileNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMapCopy, adjListCopy, 1);

		testNotEquals(whileNode, whileNodeCopy);
	}

	SECTION("Equals: Test with nested while") {
		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMap = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// While In while
			{2, WhileCFGNode::createWhileCFGNode(2)},
			// inNestedWhile
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			// outWhile
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2, 4}},
			{2, {3, 1}},
		};

		shared_ptr<CFGNode> whileNode = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		unordered_map<int, shared_ptr<CFGNode>> CFGNodesMapCopy = {
			{1, WhileCFGNode::createWhileCFGNode(1)},
			// While In while
			{2, WhileCFGNode::createWhileCFGNode(2)},
			// inNestedWhile
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			// outWhile
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
		};

		unordered_map<int, vector<int>> adjListCopy = {
			{1, {2, 4}},
			{2, {3, 1}},
		};

		shared_ptr<CFGNode> whileNodeCopy = CFGNode::createCFGFromAdjacencyList(CFGNodesMap, adjList, 1);

		testEquals(whileNode, whileNodeCopy);
	}
};
