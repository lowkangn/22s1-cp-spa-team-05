#include "catch.hpp"

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>
#include <sp/design_extractor/NextExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>

#include <memory>
#include <unordered_map>
#include <vector>

TEST_CASE("Test extract") {
	auto test = [](shared_ptr<CFGNode> nodeToExtractFrom, vector<Relationship> expectedRelations) {
		// Given
		NextExtractor extractor = NextExtractor();

		// When
		vector<Relationship> extractedRelations = extractor.extract(nodeToExtractFrom);

		// Then
		REQUIRE(extractedRelations.size() == expectedRelations.size());

		for (int i = 0; i < expectedRelations.size(); i++) {
			if (find(extractedRelations.begin(), extractedRelations.end(), expectedRelations[i]) != extractedRelations.end()) {
				REQUIRE(true);
			}
			else {
				Relationship notFound = expectedRelations[i];
				REQUIRE(false);
			}
		}
	};

	SECTION("Simple program test") {
		// Create program
		unordered_map<int, shared_ptr<CFGNode>> nodes = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3}},
			{3, {4}},
		};

		shared_ptr<CFGNode> nodeToExtractFrom = CFGNode::createCFGFromAdjacencyList(nodes, adjList, 1);

		// Next Relations
		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createAssignEntity(1), Entity::createAssignEntity(2)),
			Relationship::createNextRelationship(Entity::createAssignEntity(2), Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createAssignEntity(3), Entity::createAssignEntity(4)),
		};


		test(nodeToExtractFrom, expectedRelations);
	}

	SECTION("If program test") {
		// Create program
		unordered_map<int, shared_ptr<CFGNode>> nodes = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, IfCFGNode::createCFGNode(Entity::createIfEntity(2))},
			// Then
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Else
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			// Outside If
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3, 5}},
			{3, {4}},
			{5, {6}},
			{4, {7}},
			{6, {7}},
		};

		shared_ptr<CFGNode> nodeToExtractFrom = CFGNode::createCFGFromAdjacencyList(nodes, adjList, 1);

		// Next Relations
		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createAssignEntity(1), Entity::createIfEntity(2)),
			Relationship::createNextRelationship(Entity::createIfEntity(2), Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(2), Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createAssignEntity(3), Entity::createAssignEntity(4)),
			Relationship::createNextRelationship(Entity::createAssignEntity(5), Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createAssignEntity(4), Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(7)),
		};

		test(nodeToExtractFrom, expectedRelations);
	}

	SECTION("while program test") {
		// Create program
		unordered_map<int, shared_ptr<CFGNode>> nodes = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, WhileCFGNode::createCFGNode(Entity::createWhileEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Outside while
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3,5}},
			{3, {4}},
			{4, {2}},
		};

		shared_ptr<CFGNode> nodeToExtractFrom = CFGNode::createCFGFromAdjacencyList(nodes, adjList, 1);

		// Next Relations
		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createAssignEntity(1), Entity::createWhileEntity(2)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createAssignEntity(3), Entity::createAssignEntity(4)),
			Relationship::createNextRelationship(Entity::createAssignEntity(4), Entity::createWhileEntity(2)),
		};

		test(nodeToExtractFrom, expectedRelations);
	}

	SECTION("Nested if test") {
		// Create program
		unordered_map<int, shared_ptr<CFGNode>> nodes = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, IfCFGNode::createCFGNode(Entity::createIfEntity(2))},
			// Then
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createAssignEntity(4))},
			// Else
			{5, IfCFGNode::createCFGNode(Entity::createIfEntity(5))},
			// nested then
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
			// nested else
			{8, CFGNode::createCFGNode(Entity::createAssignEntity(8))},
			{9, CFGNode::createCFGNode(Entity::createAssignEntity(9))},
			// outside nested while (in else)
			{10, CFGNode::createCFGNode(Entity::createAssignEntity(10))},
			// Outside first while
			{11, CFGNode::createCFGNode(Entity::createAssignEntity(11))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3,5}},
			{3, {4}},
			{4, {11}},
			{5, {6,8}},
			{6, {7}},
			{7, {10}},
			{8, {9}},
			{9, {10}},
			{10, {11}},
		};

		shared_ptr<CFGNode> nodeToExtractFrom = CFGNode::createCFGFromAdjacencyList(nodes, adjList, 1);

		// Next Relations
		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createAssignEntity(1), Entity::createIfEntity(2)),
			Relationship::createNextRelationship(Entity::createIfEntity(2), Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(2), Entity::createIfEntity(5)),
			Relationship::createNextRelationship(Entity::createAssignEntity(3), Entity::createAssignEntity(4)),
			Relationship::createNextRelationship(Entity::createAssignEntity(4), Entity::createAssignEntity(11)),
			Relationship::createNextRelationship(Entity::createIfEntity(5), Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createIfEntity(5), Entity::createAssignEntity(8)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(7), Entity::createAssignEntity(10)),
			Relationship::createNextRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(9)),
			Relationship::createNextRelationship(Entity::createAssignEntity(9), Entity::createAssignEntity(10)),
			Relationship::createNextRelationship(Entity::createAssignEntity(10), Entity::createAssignEntity(11)),
		};

		test(nodeToExtractFrom, expectedRelations);
	}

	SECTION("Nested while") {
		// Create program
		unordered_map<int, shared_ptr<CFGNode>> nodes = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, WhileCFGNode::createCFGNode(Entity::createWhileEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			// Nested while
			{4, WhileCFGNode::createCFGNode(Entity::createWhileEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			// Oustide nested while
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
			// Outside while
			{8, CFGNode::createCFGNode(Entity::createAssignEntity(8))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3,8}},
			{3, {4}},
			{4, {5,7}},
			{5, {6}},
			{6, {4}},
			{7, {2}},
		};

		shared_ptr<CFGNode> nodeToExtractFrom = CFGNode::createCFGFromAdjacencyList(nodes, adjList, 1);

		// Next Relations
		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createAssignEntity(1), Entity::createWhileEntity(2)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createAssignEntity(8)),
			Relationship::createNextRelationship(Entity::createAssignEntity(3), Entity::createWhileEntity(4)),
			Relationship::createNextRelationship(Entity::createWhileEntity(4), Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createWhileEntity(4), Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(5), Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6), Entity::createWhileEntity(4)),
			Relationship::createNextRelationship(Entity::createAssignEntity(7), Entity::createWhileEntity(2)),
		};

		test(nodeToExtractFrom, expectedRelations);
	}

	SECTION("Nested if inside while") {
		// Create program
		unordered_map<int, shared_ptr<CFGNode>> nodes = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, WhileCFGNode::createCFGNode(Entity::createWhileEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createAssignEntity(3))},
			// if
			{4, IfCFGNode::createCFGNode(Entity::createIfEntity(4))},
			// then
			{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
			// else
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
			{8, CFGNode::createCFGNode(Entity::createAssignEntity(8))},
			// Oustide if
			{9, CFGNode::createCFGNode(Entity::createAssignEntity(9))},
			// Outside while
			{10, CFGNode::createCFGNode(Entity::createAssignEntity(10))},
		};

		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3, 10}},
			{3, {4}},
			{4, {5, 7}},
			{5, {6}},
			{6, {9}},
			{7, {8}},
			{8, {9}},
			{9, {2}},
		};

		shared_ptr<CFGNode> nodeToExtractFrom = CFGNode::createCFGFromAdjacencyList(nodes, adjList, 1);

		// Next Relations
		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createAssignEntity(1), Entity::createWhileEntity(2)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createAssignEntity(10)),
			Relationship::createNextRelationship(Entity::createAssignEntity(3), Entity::createIfEntity(4)),
			Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(5), Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(9)),
			Relationship::createNextRelationship(Entity::createAssignEntity(7), Entity::createAssignEntity(8)),
			Relationship::createNextRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(9)),
			Relationship::createNextRelationship(Entity::createAssignEntity(9), Entity::createWhileEntity(2)),
		};

		test(nodeToExtractFrom, expectedRelations);
	}
}


