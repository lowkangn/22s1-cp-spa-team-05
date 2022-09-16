#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesSClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::generateStmtArg("s");
		ClauseArgument rhs = ClauseArgument::generateVariableArg("v");
		ModifiesSClause modifiesSClause = ModifiesSClause(lhs, rhs);

		// when
		bool actual = modifiesSClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::generateStmtArg("s");
	ClauseArgument secondStmtArg = ClauseArgument::generateStmtArg("s1");
	ClauseArgument firstVarArg = ClauseArgument::generateVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::generateVariableArg("v1");

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

	ClauseArgument procArg = ClauseArgument::generateProcedureArg("p");
	ClauseArgument assignArg = ClauseArgument::generateAssignArg("a");
	ClauseArgument constArg = ClauseArgument::generateConstantArg("4");
	ClauseArgument readArg = ClauseArgument::generateReadArg("r");
	ClauseArgument printArg = ClauseArgument::generatePrintArg("pr");
	ClauseArgument callArg = ClauseArgument::generateCallArg("c");
	ClauseArgument whileArg = ClauseArgument::generateWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::generateIfArg("i");
	ClauseArgument lineNumArg = ClauseArgument::generateLineNumberArg("5");
	ClauseArgument stringLitArg = ClauseArgument::generateStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::generateWildcardArg();

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
