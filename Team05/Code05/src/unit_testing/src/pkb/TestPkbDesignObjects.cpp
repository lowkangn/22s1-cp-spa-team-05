#include "catch.hpp"

#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/design_objects/patterns/PkbStatementPattern.h>

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


}

TEST_CASE("PkbStatementPattern::getKey") {
	auto test = [](shared_ptr<PkbStatementPattern> pattern, string expectedKey) {
		// given, when, then
		REQUIRE(pattern->getKey() == expectedKey);
	};

	SECTION("Assign pattern") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		shared_ptr<PkbStatementEntity> assignStatement = shared_ptr<PkbStatementEntity>(PkbStatementEntity::createAssignStatementEntity(1));
		shared_ptr<PkbStatementPattern> pattern = PkbStatementPattern::createAssignPattern(1, strings);
		string expectedKey = "xx+1" + assignStatement->getKey();
		test(pattern, expectedKey);
	};

}

TEST_CASE("PkbStatementPattern::regexMatch") {
	// test on https://regex101.com/
	auto test = [](shared_ptr<PkbStatementPattern> pattern, vector<string> regexStringsToMatch, bool shouldThrow, bool shouldMatch) {
		// given, when, then
		if (shouldThrow) {
			REQUIRE_THROWS_AS(pattern->isRegexMatch(regexStringsToMatch), PkbException);
		}
		else {
			REQUIRE(pattern->isRegexMatch(regexStringsToMatch) == shouldMatch);
		}
	};


	SECTION("Should match, _; _+_") {
		vector<string> strings = {
			"x",
			"x+1"
		};
		vector<string> regexStringsToMatch = {
			".*",
			".*\\+.*" // escape the + character
		};
		bool shouldThrow = false;
		bool shouldMatch = true;
		shared_ptr<PkbStatementPattern> pattern = PkbStatementPattern::createAssignPattern(1, strings);

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
		shared_ptr<PkbStatementPattern> pattern = PkbStatementPattern::createAssignPattern(1, strings);

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
		shared_ptr<PkbStatementPattern> pattern = PkbStatementPattern::createAssignPattern(1, strings);

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
		shared_ptr<PkbStatementPattern> pattern = PkbStatementPattern::createAssignPattern(1, strings);

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
		shared_ptr<PkbStatementPattern> pattern = PkbStatementPattern::createAssignPattern(1, strings);

		test(pattern, regexStringsToMatch, shouldThrow, shouldMatch);
	};
}
