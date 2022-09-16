#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesSClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		ModifiesSClause modifiesSClause = ModifiesSClause(lhs, rhs);

		// when
		bool actual = modifiesSClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");

	SECTION("Equal") {
		RelationshipClause* modifiesSClauseAgain = new ModifiesSClause(firstStmtArg, firstVarArg);
		testEquals(modifiesSClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		RelationshipClause* modifiesSClauseDiffLhsString = new ModifiesSClause(secondStmtArg, firstVarArg);
		testEquals(modifiesSClauseDiffLhsString, false);

		RelationshipClause* modifiesSClauseDiffRhsString = new ModifiesSClause(firstStmtArg, secondVarArg);
		testEquals(modifiesSClauseDiffRhsString, false);

		RelationshipClause* modifiesSClauseDiffStrings = new ModifiesSClause(secondStmtArg, secondVarArg);
		testEquals(modifiesSClauseDiffStrings, false);
	}

	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument callArg = ClauseArgument::createCallArg("c");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	list<ClauseArgument> otherArguments = list<ClauseArgument>{procArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg};

	SECTION("Not equal, different types") {
		for (ClauseArgument argument: otherArguments) {
			RelationshipClause* modifiesSClauseDiffLhsArg = new ModifiesSClause(firstStmtArg, argument);
			testEquals(modifiesSClauseDiffLhsArg, false);

			RelationshipClause* modifiesSClauseDiffRhsArg = new ModifiesSClause(argument, firstVarArg);
			testEquals(modifiesSClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a ModifiesSClause") {
		RelationshipClause* modifiesPClause = new ModifiesPClause(procArg, firstVarArg);
		testEquals(modifiesPClause, false);

		// TODO: Add more when other Clause classes are implemented
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("ModifiesSClause: test execute") {}
