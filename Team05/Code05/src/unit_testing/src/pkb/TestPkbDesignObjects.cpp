#include "catch.hpp"

#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/design_objects/patterns/PkbAssignPattern.h>
#include <pkb/design_objects/patterns/PkbIfPattern.h>
#include <pkb/design_objects/patterns/PkbWhilePattern.h>
#include <pkb/design_objects/graphs/PkbGraphNode.h>
#include <pkb/design_objects/graphs/PkbControlFlowGraphNode.h>

#include <pkb/PkbException.h>

#include <memory>
#include <string>
using namespace std;

TEST_CASE("PkbEntity: test ::getLineNumber") {
	auto test = [](shared_ptr<PkbEntity> entity, bool throws, int expectedLineNumber) {
		// given, when, then
		if (throws) {
			REQUIRE_THROWS_AS(entity->getLineNumber(), PkbException);
		}
		else {
			REQUIRE(entity->getLineNumber() == expectedLineNumber);
		}
	};

	SECTION("procedure entity throws") {
		string identifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbProcedureEntity(identifier));
		bool throws = true;
		int expectedLineNumber = -1;
		test(entity, throws, expectedLineNumber);
	};

	SECTION("variable entity throws") {
		string identifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbVariableEntity(identifier));
		bool throws = true;
		int expectedLineNumber = -1;
		test(entity, throws, expectedLineNumber);
	};

	SECTION("statement entity correct") {
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(1));
		bool throws = false;
		int expectedLineNumber = 1;
		test(entity, throws, expectedLineNumber);
	};
}

TEST_CASE("PkbEntity: test ::getIdentifier") {
	auto test = [](shared_ptr<PkbEntity> entity, bool throws, string expectedIdentifier) {
		// given, when, then
		if (throws) {
			REQUIRE_THROWS_AS(entity->getIdentifier(), PkbException);
		}
		else {
			REQUIRE(entity->getIdentifier() == expectedIdentifier);
		}
	};

	SECTION("procedure entity correct") {
		string expectedIdentifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbProcedureEntity(expectedIdentifier));
		bool throws = false;
		test(entity, throws, expectedIdentifier);
	};

	SECTION("variable entity correct") {
		string expectedIdentifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbVariableEntity(expectedIdentifier));
		bool throws = false;
		test(entity, throws, expectedIdentifier);
	};

	SECTION("statement entity throws") {
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(1));
		bool throws = true;
		string expectedIdentifier = "dummy";
		test(entity, throws, expectedIdentifier);
	};

}

TEST_CASE("PkbEntity: test ::getKey") {
	auto test = [](shared_ptr<PkbEntity> entity, string expectedKey) {
		// given, when, then
		REQUIRE(entity->getKey() == expectedKey);
	};

	SECTION("Statement, expect statement number") {
		string expectedKey = "1";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(1));
		test(entity, expectedKey);
	};

	SECTION("Variable, expect name") {
		string expectedKey = "variable";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbVariableEntity(expectedKey));
		test(entity, expectedKey);
	};

	SECTION("Procedure, expect name") {
		string expectedKey = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbProcedureEntity(expectedKey));
		test(entity, expectedKey);
	}
}

TEST_CASE("PkbRelationship: test ::getKey") {
	auto test = [](shared_ptr<PkbRelationship> relationship, string expectedKey) {
		// given, when, then
		REQUIRE(relationship->getKey() == expectedKey);
	};

	// create lhs and rhs, to be shared
	string lhsKey = "procedure";
	string rhsKey = "variable";
	shared_ptr<PkbEntity> lhs = shared_ptr<PkbEntity>(new PkbProcedureEntity(lhsKey));
	shared_ptr<PkbEntity> rhs = shared_ptr<PkbEntity>(new PkbVariableEntity(rhsKey));

	// tests
	SECTION("Follows") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbFollowsRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("0");
		test(r, expectedKey);
	};
	SECTION("FollowsStar") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("1");
		test(r, expectedKey);
	};
	SECTION("Parent") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbParentRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("2");
		test(r, expectedKey);
	};
	SECTION("ParentStar") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbParentStarRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("3");
		test(r, expectedKey);
	};
	SECTION("Uses") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbUsesRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("4");
		test(r, expectedKey);
	};
	SECTION("Modifies") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("5");
		test(r, expectedKey);
	};
	SECTION("Calls") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbCallsRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("6");
		test(r, expectedKey);
	};
	SECTION("CallsStar") {
		shared_ptr<PkbRelationship> r = shared_ptr<PkbRelationship>(new PkbCallsStarRelationship(lhs, rhs));
		string expectedKey = lhsKey + rhsKey + string("7");
		test(r, expectedKey);
	};
}

TEST_CASE("PkbAssignPattern throw error on creation with != 2 strings") {
	auto test = [](vector<string> strings, bool shouldThrow) {
		if (shouldThrow) {
			REQUIRE_THROWS_AS(PkbAssignPattern::createAssignPattern(1, strings), PkbException);
		}
		else {
			REQUIRE_NOTHROW(PkbAssignPattern::createAssignPattern(1, strings));
		}
	};

	SECTION("No throw for 2 strings") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		bool shouldThrow = false;
		test(strings, shouldThrow);
	}

	SECTION("Throw for 3 strings") {
		vector<string> strings = {
			"x",
			"x+1",
			"y+2"
		};
		bool shouldThrow = true;
		test(strings, shouldThrow);
	}

	SECTION("Throw for 1 strings") {
		vector<string> strings = {
			"x",
			"x+1",
			"y+2"
		};
		bool shouldThrow = true;
		test(strings, shouldThrow);
	}
}

TEST_CASE("PkbIfPattern throw error on creation with != 1 strings") {
	auto test = [](vector<string> strings, bool shouldThrow) {
		if (shouldThrow) {
			REQUIRE_THROWS_AS(PkbIfPattern::createIfPattern(1, strings), PkbException);
		}
		else {
			REQUIRE_NOTHROW(PkbIfPattern::createIfPattern(1, strings));
		}
	};

	SECTION("No throw for 1 string") {
		vector<string> strings = {
			"x",
		};
		bool shouldThrow = false;
		test(strings, shouldThrow);
	}

	SECTION("Throw for 2 strings") {
		vector<string> strings = {
			"x",
			"x+1",
			"y+2"
		};
		bool shouldThrow = true;
		test(strings, shouldThrow);
	}

	SECTION("Throw for 0 strings") {
		vector<string> strings = { };
		bool shouldThrow = true;
		test(strings, shouldThrow);
	}
}

TEST_CASE("PkbWhilePattern throw error on creation with != 1 strings") {
	auto test = [](vector<string> strings, bool shouldThrow) {
		if (shouldThrow) {
			REQUIRE_THROWS_AS(PkbWhilePattern::createWhilePattern(1, strings), PkbException);
		}
		else {
			REQUIRE_NOTHROW(PkbWhilePattern::createWhilePattern(1, strings));
		}
	};

	SECTION("No throw for 1 string") {
		vector<string> strings = {
			"x",
		};
		bool shouldThrow = false;
		test(strings, shouldThrow);
	}

	SECTION("Throw for 2 strings") {
		vector<string> strings = {
			"x",
			"x+1",
			"y+2"
		};
		bool shouldThrow = true;
		test(strings, shouldThrow);
	}

	SECTION("Throw for 0 strings") {
		vector<string> strings = { };
		bool shouldThrow = true;
		test(strings, shouldThrow);
	}
}

TEST_CASE("PkbPattern::getKey") {
	auto test = [](shared_ptr<PkbPattern> pattern, string expectedKey) {
		// given, when, then
		REQUIRE(pattern->getKey() == expectedKey);
	};

	SECTION("Assign pattern") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		shared_ptr<PkbStatementEntity> assignStatement = shared_ptr<PkbStatementEntity>(PkbStatementEntity::createAssignStatementEntity(1));
		shared_ptr<PkbAssignPattern> pattern = PkbAssignPattern::createAssignPattern(1, strings);
		string expectedKey = "xx+1" + assignStatement->getKey();
		test(pattern, expectedKey);
	};

	SECTION("If pattern") {
		vector<string> strings = {
			"count"
		};
		shared_ptr<PkbStatementEntity> ifStatement = shared_ptr<PkbStatementEntity>(PkbStatementEntity::createIfStatementEntity(1));
		shared_ptr<PkbIfPattern> pattern = PkbIfPattern::createIfPattern(1, strings);
		string expectedKey = "count" + ifStatement->getKey();
		test(pattern, expectedKey);
	};
	SECTION("While pattern") {
		vector<string> strings = {
			"count"
		};
		shared_ptr<PkbStatementEntity> whileStatement = shared_ptr<PkbStatementEntity>(PkbStatementEntity::createWhileStatementEntity(1));
		shared_ptr<PkbWhilePattern> pattern = PkbWhilePattern::createWhilePattern(1, strings);
		string expectedKey = "count" + whileStatement->getKey();
		test(pattern, expectedKey);
	};
}

TEST_CASE("PkbAssignPattern::regexMatch") {
	// test on https://regex101.com/
	auto test = [](shared_ptr<PkbAssignPattern> pattern, vector<string> regexStringsToMatch, bool shouldThrow, bool shouldMatch) {
		// given, when, then
		if (shouldThrow) {
			REQUIRE_THROWS_AS(pattern->isRegexMatch(regexStringsToMatch), PkbException);
		}
		else {
			REQUIRE(pattern->isRegexMatch(regexStringsToMatch) == shouldMatch);
		}
	};


	SECTION("Should match, _; _x_ with nested") {
		vector<string> strings = {
			"x",
			"yx*1+" // is y * x + 1
 		};
		vector<string> regexStringsToMatch = {
			".*",
			".*x.*" // escape the + character
		};
		bool shouldThrow = false;
		bool shouldMatch = true;
		shared_ptr<PkbAssignPattern> pattern = PkbAssignPattern::createAssignPattern(1, strings);

		test(pattern, regexStringsToMatch, shouldThrow, shouldMatch);
	};

	SECTION("Should match, _; _x_") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		vector<string> regexStringsToMatch = {
			".*",
			".*x.*"
		};
		bool shouldThrow = false;
		bool shouldMatch = true;
		shared_ptr<PkbAssignPattern> pattern = PkbAssignPattern::createAssignPattern(1, strings);

		test(pattern, regexStringsToMatch, shouldThrow, shouldMatch);
	};

	SECTION("Should match, x; _") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		vector<string> regexStringsToMatch = {
			"x",
			".*"
		};
		bool shouldThrow = false;
		bool shouldMatch = true;
		shared_ptr<PkbAssignPattern> pattern = PkbAssignPattern::createAssignPattern(1, strings);

		test(pattern, regexStringsToMatch, shouldThrow, shouldMatch);
	};

	SECTION("Should throw due to length mismatch") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		vector<string> regexStringsToMatch = {
		};
		bool shouldThrow = true;
		bool shouldMatch = true;
		shared_ptr<PkbAssignPattern> pattern = PkbAssignPattern::createAssignPattern(1, strings);

		test(pattern, regexStringsToMatch, shouldThrow, shouldMatch);
	};

	SECTION("Should not match, _;x") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		vector<string> regexStringsToMatch = {
			".*",
			"x"
		};
		bool shouldThrow = false;
		bool shouldMatch = false;
		shared_ptr<PkbAssignPattern> pattern = PkbAssignPattern::createAssignPattern(1, strings);

		test(pattern, regexStringsToMatch, shouldThrow, shouldMatch);
	};
}

TEST_CASE("PkbIfPattern::regexMatch, PkbWhilePattern::regexMatch") {
	/*
		Group if and while pattern together for tests due to similar behaviour
	*/
	auto test = [](shared_ptr<PkbPattern> pattern, vector<string> regexStringsToMatch, bool shouldThrow, bool shouldMatch) {
		// given, when, then
		if (shouldThrow) {
			REQUIRE_THROWS_AS(pattern->isRegexMatch(regexStringsToMatch), PkbException);
		}
		else {
			REQUIRE(pattern->isRegexMatch(regexStringsToMatch) == shouldMatch);
		}
	};

	SECTION("Should match count exactly") {
		vector<string> strings = {
			"count",
		};
		vector<string> regexStringsToMatch = {
			"count",
		};
		bool shouldThrow = false;
		bool shouldMatch = true;
		shared_ptr<PkbIfPattern> ifPattern = PkbIfPattern::createIfPattern(1, strings);
		shared_ptr<PkbWhilePattern> whilePattern = PkbWhilePattern::createWhilePattern(2, strings);

		test(ifPattern, regexStringsToMatch, shouldThrow, shouldMatch);
		test(whilePattern, regexStringsToMatch, shouldThrow, shouldMatch);
	}

	SECTION("Should match count exactly using regex tokens") {
		vector<string> strings = {
			"count",
		};
		vector<string> regexStringsToMatch = {
			"^count$"
		};
		bool shouldThrow = false;
		bool shouldMatch = true;
		shared_ptr<PkbIfPattern> ifPattern = PkbIfPattern::createIfPattern(1, strings);
		shared_ptr<PkbWhilePattern> whilePattern = PkbWhilePattern::createWhilePattern(2, strings);

		test(ifPattern, regexStringsToMatch, shouldThrow, shouldMatch);
		test(whilePattern, regexStringsToMatch, shouldThrow, shouldMatch);
	}
	
	SECTION("Should not match x") {
		vector<string> strings = {
			"xCount",
		};
		vector<string> regexStringsToMatch = {
			"x"
		};
		bool shouldThrow = false;
		bool shouldMatch = false;
		shared_ptr<PkbIfPattern> ifPattern = PkbIfPattern::createIfPattern(1, strings);
		shared_ptr<PkbWhilePattern> whilePattern = PkbWhilePattern::createWhilePattern(2, strings);

		test(ifPattern, regexStringsToMatch, shouldThrow, shouldMatch);
		test(whilePattern, regexStringsToMatch, shouldThrow, shouldMatch);
	}

	SECTION("Should not match xCount using regex tokens") {
		vector<string> strings = {
			"xCount",
		};
		vector<string> regexStringsToMatch = {
			"^count$"
		};
		bool shouldThrow = false;
		bool shouldMatch = false;
		shared_ptr<PkbIfPattern> ifPattern = PkbIfPattern::createIfPattern(1, strings);
		shared_ptr<PkbWhilePattern> whilePattern = PkbWhilePattern::createWhilePattern(2, strings);

		test(ifPattern, regexStringsToMatch, shouldThrow, shouldMatch);
		test(whilePattern, regexStringsToMatch, shouldThrow, shouldMatch);
	}

	// IF pattern will be pattern(v, _, _), so only one regex string to match
	SECTION("Should throw due to too many regex/pattern args") {
		vector<string> strings = {
			"count",
		};
		vector<string> regexStringsToMatch = {
			"count",
			"x"
		};
		bool shouldThrow = true;
		bool shouldMatch = true;
		shared_ptr<PkbIfPattern> ifPattern = PkbIfPattern::createIfPattern(1, strings);
		shared_ptr<PkbWhilePattern> whilePattern = PkbWhilePattern::createWhilePattern(2, strings);

		test(ifPattern, regexStringsToMatch, shouldThrow, shouldMatch);
		test(whilePattern, regexStringsToMatch, shouldThrow, shouldMatch);
	}

	SECTION("Should match single wildcard") {
		vector<string> strings = {
			"count",
		};
		vector<string> regexStringsToMatch = {
			".*",
		};
		bool shouldThrow = false;
		bool shouldMatch = true;
		shared_ptr<PkbIfPattern> ifPattern = PkbIfPattern::createIfPattern(1, strings);
		shared_ptr<PkbWhilePattern> whilePattern = PkbWhilePattern::createWhilePattern(2, strings);

		test(ifPattern, regexStringsToMatch, shouldThrow, shouldMatch);
		test(whilePattern, regexStringsToMatch, shouldThrow, shouldMatch);
	}

	SECTION("Should throw trying to match multiple wildcards") {
		vector<string> strings = {
			"count",
		};
		vector<string> regexStringsToMatch = {
			".*",
			".*",
			".*"
		};
		bool shouldThrow = true;
		bool shouldMatch = true;
		shared_ptr<PkbIfPattern> ifPattern = PkbIfPattern::createIfPattern(1, strings);
		shared_ptr<PkbWhilePattern> whilePattern = PkbWhilePattern::createWhilePattern(2, strings);

		test(ifPattern, regexStringsToMatch, shouldThrow, shouldMatch);
		test(whilePattern, regexStringsToMatch, shouldThrow, shouldMatch);
	}
	
}

TEST_CASE("PkbGraphNode: add and get neighbours") {
	auto test = [](shared_ptr<PkbGraphNode> root, vector<shared_ptr<PkbGraphNode>> children) {
		// given 
		for (shared_ptr<PkbGraphNode> c : children) {
			root->addNeighbour(c);
		}

		// when
		vector<shared_ptr<PkbGraphNode>> neighbours = root->getNeighbours();

		// then
		for (shared_ptr<PkbGraphNode> c : children) {
			REQUIRE(find(neighbours.begin(), neighbours.end(), c) != neighbours.end()); // is inside
		}
	};

	// create one of each node type 
	vector<shared_ptr<PkbGraphNode>> children = {
		PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(2)),
		PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(3)),
		PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(4)),
		PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5)),
		PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(6)),
		PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(7)),
	};

	shared_ptr<PkbGraphNode> root = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1));

	// test
	test(root, children);
	
}

TEST_CASE("PkbGraphNode: equals") {
	auto test = [](shared_ptr<PkbGraphNode> one, shared_ptr<PkbGraphNode> other, bool expected) {
		// given and when
		bool isEquals = one->equals(other);

		// then
		REQUIRE(isEquals == expected);
	};

	SECTION("Same entity") {
		shared_ptr<PkbGraphNode> one = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(2));
		shared_ptr<PkbGraphNode> other = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(2));
		bool expected = true;
		test(one, other, expected);
	}

	SECTION("Same type but different") {
		shared_ptr<PkbGraphNode> one = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(4));
		shared_ptr<PkbGraphNode> other = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(3));
		bool expected = false;
		test(one, other, expected);
	}

	SECTION("different") {
		shared_ptr<PkbGraphNode> one = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5));
		shared_ptr<PkbGraphNode> other = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(6));
		bool expected = false;
		test(one, other, expected);
	}

	SECTION("full graph") {
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

		// graph 1
		vector<vector<int>> adjList1 = { {1}, {2}, {3,4}, {5}, {5}, {6}, {5, 7}, {7} };
		unordered_map<int, shared_ptr<PkbGraphNode>> nodeIdToNode1 = {
			{0, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1))},
			{1, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(2))},
			{2, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(3))},
			{3, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(4))},
			{4, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5))},
			{5, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(6))},
			{6, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(7))},
			{7, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(8))},
		};
		shared_ptr<PkbGraphNode> one = PkbGraphNode::createGraphFromAdjList(0, adjList1, nodeIdToNode1);

		// graph 2
		vector<vector<int>> adjList2 = { {1}, {2}, {3,4}, {5}, {5}, {6}, {5, 7}, {7} };
		unordered_map<int, shared_ptr<PkbGraphNode>> nodeIdToNode2 = {
			{0, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(1))},
			{1, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(2))},
			{2, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(3))},
			{3, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(4))},
			{4, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(5))},
			{5, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(6))},
			{6, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(7))},
			{7, PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(8))},
		};
		shared_ptr<PkbGraphNode> other = PkbGraphNode::createGraphFromAdjList(0, adjList2, nodeIdToNode2);
		bool expected = true;
		test(one, other, expected);
	}
}

TEST_CASE("PkbControlFlowGraphNode: getKey") {
	auto test = [](shared_ptr<PkbGraphNode> node, string expectedKey) {
		// given and when
		string key = node->getKey();

		// then
		REQUIRE(key == expectedKey);
	};


	// one test for each type
	SECTION("Assign") {
		shared_ptr<PkbGraphNode> node = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createAssignStatementEntity(2));
		string expectedKey = PkbStatementEntity::createAssignStatementEntity(2)->getKey();
		test(node, expectedKey);
	}

	SECTION("Calls") {
		shared_ptr<PkbGraphNode> node = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createCallStatementEntity(2));
		string expectedKey = PkbStatementEntity::createCallStatementEntity(2)->getKey();
		test(node, expectedKey);
	}

	SECTION("If") {
		shared_ptr<PkbGraphNode> node = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createIfStatementEntity(2));
		string expectedKey = PkbStatementEntity::createIfStatementEntity(2)->getKey();
		test(node, expectedKey);
	}

	SECTION("print") {
		shared_ptr<PkbGraphNode> node = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createPrintStatementEntity(2));
		string expectedKey = PkbStatementEntity::createPrintStatementEntity(2)->getKey();
		test(node, expectedKey);
	}

	SECTION("read") {
		shared_ptr<PkbGraphNode> node = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createReadStatementEntity(2));
		string expectedKey = PkbStatementEntity::createReadStatementEntity(2)->getKey();
		test(node, expectedKey);
	}

	SECTION("while") {
		shared_ptr<PkbGraphNode> node = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(PkbStatementEntity::createWhileStatementEntity(2));
		string expectedKey = PkbStatementEntity::createWhileStatementEntity(2)->getKey();
		test(node, expectedKey);
	}
}



