#include <catch.hpp>

#include <pkb/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <pkb/design_objects/graphs/PkbGraphNode.h>
#include <pkb/graph_managers/PkbGraphManager.h>
#include <pkb/PkbException.h>
#include <iostream>

TEST_CASE("PkbGraphManager: getNode and BFS initialization") {
	auto test = [](string key, shared_ptr<PkbGraphManager> manager, shared_ptr<PkbGraphNode> expectedNode, bool notInside) {
		// given and when
		shared_ptr<PkbGraphNode> gotten = manager->getNode(key);

		// then
		if (notInside) {
			REQUIRE(!gotten); // is falsey
		}
		else {
			if (!gotten->equals(expectedNode)) {
				cout << key << " " << expectedNode->getKey() << std::endl;
			}
			REQUIRE(gotten->equals(expectedNode));
		}
	};

	// 1. initialize graph
	/*
		1	x = 1; // assign
		2	call a; // call
		3	if (x == 1) then { // if
		4		read b; // read
			} else {
		5		print x; // print
			}
		6	while (x == 1) { // while
		7		x = 0; // assign
			}
		8	x = 1;

	*/
	
	vector<vector<int>> adjList = { {1}, {2}, {3,4}, {5}, {5}, {6}, {5, 7}, {7} };
	unordered_map<int, shared_ptr<PkbGraphNode>> nodeIdToNode = {
		{0, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1))},
		{1, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(2))},
		{2, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(3))},
		{3, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(4))},
		{4, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5))},
		{5, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(6))},
		{6, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(7))},
		{7, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(8))},
	};
	shared_ptr<PkbGraphNode> graph = PkbGraphNode::createGraphFromAdjList(0, adjList, nodeIdToNode);

	// 2. initialize manager by bfs
	shared_ptr<PkbGraphManager> manager = shared_ptr<PkbGraphManager>(new PkbGraphManager(graph));

	SECTION("node inside") {
		shared_ptr<PkbGraphNode> expectedNode = nodeIdToNode[0]; // because of deep equality, need to create
		string key = expectedNode->getKey();
		test(key, manager, expectedNode, false);
	}

	SECTION("node not inside") {
		shared_ptr<PkbGraphNode> expectedNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(14));
		string key = expectedNode->getKey();
		test(key, manager, expectedNode, true);
	}
}

TEST_CASE("PkbGraphManager: getImmediateNeighbours") {
	auto test = [](string key, shared_ptr<PkbGraphManager> manager, vector<shared_ptr<PkbGraphNode>> expectedNeighbours, bool shouldThrow) {
		// given, when, then
		if (shouldThrow) {
			REQUIRE_THROWS_AS(manager->getImmediateNeighbours(key), PkbException);
		}
		else {
			vector<shared_ptr<PkbGraphNode>> neighbours = manager->getImmediateNeighbours(key);
			for (shared_ptr<PkbGraphNode> e : expectedNeighbours) {
				bool found = false;
				for (shared_ptr<PkbGraphNode> n : neighbours) {
					if (n->equals(e)) {
						found = true;
						break;
					}
				}
				if (!found) {
					for (shared_ptr<PkbGraphNode> n : neighbours) {
						cout << n->getKey() << std::endl;
					}
					
				}
				REQUIRE(found);
			}
		}
	};

	// 1. initialize graph
	/*
		1	x = 1; // assign
		2	call a; // call
		3	if (x == 1) then { // if
		4		read b; // read
			} else {
		5		print x; // print
			}
		6	while (x == 1) { // while
		7		x = 0; // assign
			}
		8	x = 1;

	*/

	vector<vector<int>> adjList = { {1}, {2}, {3,4}, {5}, {5}, {6}, {5, 7}, {7} };
	unordered_map<int, shared_ptr<PkbGraphNode>> nodeIdToNode = {
		{0, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1))},
		{1, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(2))},
		{2, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(3))},
		{3, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(4))},
		{4, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5))},
		{5, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(6))},
		{6, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(7))},
		{7, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(8))},
	};
	shared_ptr<PkbGraphNode> graph = PkbGraphNode::createGraphFromAdjList(0, adjList, nodeIdToNode);

	// 2. initialize manager by bfs
	shared_ptr<PkbGraphManager> manager = shared_ptr<PkbGraphManager>(new PkbGraphManager(graph));

	SECTION("inside, should be correct") {
		string key = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1))->getKey();
		vector<shared_ptr<PkbGraphNode>> expectedNeighbours = {
			 nodeIdToNode[1],
		};
		test(key, manager, expectedNeighbours, false);
	}

	SECTION("inside, should be correct, more neighbours") {
		string key = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(3))->getKey();
		vector<shared_ptr<PkbGraphNode>> expectedNeighbours = {
			 nodeIdToNode[3],
			 nodeIdToNode[4]
		};
		test(key, manager, expectedNeighbours, false);
	}

	SECTION("not inside, should throw") {
		string key = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(23))->getKey();
		vector<shared_ptr<PkbGraphNode>> expectedNeighbours = {};
		test(key, manager, expectedNeighbours, true);
	}
}


TEST_CASE("PkbGraphManager: canReachBFromNodeA") {
	auto test = [](string key1, string key2, shared_ptr<PkbGraphManager> manager, bool expectedResult, bool shouldThrow) {
		if (shouldThrow) {
			REQUIRE_THROWS_AS(manager->canReachNodeBFromNodeA(key1, key2), PkbException);
		}
		else {
			// given, when, then
			bool result = manager->canReachNodeBFromNodeA(key1, key2);

			REQUIRE(result == expectedResult);
		}
		
	};

	// 1. initialize graph
	/*
		1	x = 1; // assign
		2	call a; // call
		3	if (x == 1) then { // if
		4		read b; // read
			} else {
		5		print x; // print
			}
		6	while (x == 1) { // while
		7		x = 0; // assign
			}
		8	x = 1;

	*/


	vector<vector<int>> adjList = { {1}, {2}, {3,4}, {5}, {5}, {6}, {5, 7}, {7} };
	unordered_map<int, shared_ptr<PkbGraphNode>> nodeIdToNode = {
		{0, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1))},
		{1, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(2))},
		{2, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(3))},
		{3, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(4))},
		{4, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5))},
		{5, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(6))},
		{6, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(7))},
		{7, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(8))},
	};
	shared_ptr<PkbGraphNode> graph = PkbGraphNode::createGraphFromAdjList(0, adjList, nodeIdToNode);

	// 2. initialize manager by bfs
	shared_ptr<PkbGraphManager> manager = shared_ptr<PkbGraphManager>(new PkbGraphManager(graph));

	SECTION("not inside, should throw") {
		string key1 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(23))->getKey();
		string key2 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1))->getKey();
		test(key1, key2, manager, false, true);
	}

	SECTION("going backwards, should return false") {
		string key1 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(2))->getKey();
		string key2 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(1))->getKey();
		test(key1, key2, manager, false, false);
	}

	SECTION("going backwards in while loop, should return true") {
		string key1 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(7))->getKey();
		string key2 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(6))->getKey();
		test(key1, key2, manager, true, false);
	}

	SECTION("if can reach all children") {
		string key1 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(3))->getKey();
		string key2 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(4))->getKey();
		string key3 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5))->getKey();
		string key4 = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(6))->getKey();
		// if -> if
		test(key1, key2, manager, true, false);

		// if -> else 
		test(key1, key2, manager, true, false);

		// if -> after
		test(key1, key2, manager, true, false);

	}
}
