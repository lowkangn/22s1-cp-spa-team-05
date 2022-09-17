#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectClause: test equals") {
	auto testEquals = [](const SelectClause* other, bool expected) {
		// given
		SelectClause selectClause = SelectClause(ClauseArgument::createVariableArg("v"));

		// when
		bool actual = selectClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument assignArgSameIdentifier = ClauseArgument::createAssignArg("v");
	ClauseArgument varArgDifferentIdentifier = ClauseArgument::createVariableArg("a");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");

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
