#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/SelectClause.cpp>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectClause: test equals") {
	auto testEquals = [](const SelectClause* other, bool expected) {
		// given
		SelectClause selectClause = SelectClause(ClauseArgument("v", ArgumentType::VARIABLE));

		// when
		bool actual = selectClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument assignArgSameIdentifier = ClauseArgument("v", ArgumentType::ASSIGN);
	ClauseArgument varArgDifferentIdentifier = ClauseArgument("a", ArgumentType::VARIABLE);
	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);

	SelectClause* firstSelectClause = new SelectClause(varArg);
	SelectClause* secondSelectClause = new SelectClause(assignArgSameIdentifier);
	SelectClause* thirdSelectClause = new SelectClause(varArgDifferentIdentifier);
	SelectClause* fourthSelectClause = new SelectClause(assignArg);

	SECTION("Equal") {
		testEquals(firstSelectClause, true);
	}

	SECTION("Not equal") {
		testEquals(secondSelectClause, false);
		testEquals(thirdSelectClause, false);
		testEquals(fourthSelectClause, false);
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("SelectClause: test execute") {}
