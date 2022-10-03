#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectClause: test equals") {
	auto testEquals = [](const SelectClause* other, bool expected) {
		// given
		ClauseArgument varArg = ClauseArgument::createVariableArg("v");
		SelectClause selectClause = SelectClause::createSynonymSelectClause({varArg});

		// when
		bool actual = selectClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument assignArgSameIdentifier = ClauseArgument::createAssignArg("v");
	ClauseArgument varArgDifferentIdentifier = ClauseArgument::createVariableArg("a");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");

	SelectClause firstSelectClause = SelectClause::createSynonymSelectClause({varArg});
	SelectClause* firstSelectClausePointer = &firstSelectClause;
	SelectClause secondSelectClause = SelectClause::createSynonymSelectClause({assignArgSameIdentifier});
	SelectClause* secondSelectClausePointer = &secondSelectClause;
	SelectClause thirdSelectClause = SelectClause::createSynonymSelectClause({varArgDifferentIdentifier});
	SelectClause* thirdSelectClausePointer = &thirdSelectClause;
	SelectClause fourthSelectClause = SelectClause::createSynonymSelectClause({assignArg});
	SelectClause* fourthSelectClausePointer = &fourthSelectClause;

	SECTION("Equal") {
		testEquals(firstSelectClausePointer, true);
	}

	SECTION("Not equal") {
		testEquals(secondSelectClausePointer, false);
		testEquals(thirdSelectClausePointer, false);
		testEquals(fourthSelectClausePointer, false);
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("SelectClause: test execute") {}
