#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesPClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::generateProcedureArg("p");
		ClauseArgument rhs = ClauseArgument::generateVariableArg("v");
		ModifiesPClause modifiesPClause = ModifiesPClause(lhs, rhs);

		// when
		bool actual = modifiesPClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcArg = ClauseArgument::generateProcedureArg("p");
	ClauseArgument secondProcArg = ClauseArgument::generateProcedureArg("p1");
	ClauseArgument firstVarArg = ClauseArgument::generateVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::generateVariableArg("v1");

	SECTION("Equal") {
		RelationshipClause* modifiesPClauseAgain = new ModifiesPClause(firstProcArg, firstVarArg);
		testEquals(modifiesPClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		RelationshipClause* modifiesPClauseDiffLhsString = new ModifiesPClause(secondProcArg, firstVarArg);
		testEquals(modifiesPClauseDiffLhsString, false);

		RelationshipClause* modifiesPClauseDiffRhsString = new ModifiesPClause(firstProcArg, secondVarArg);
		testEquals(modifiesPClauseDiffRhsString, false);

		RelationshipClause* modifiesPClauseDiffStrings = new ModifiesPClause(secondProcArg, secondVarArg);
		testEquals(modifiesPClauseDiffStrings, false);
	}

	ClauseArgument stmtArg = ClauseArgument::generateStmtArg("s");
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

	list<ClauseArgument> otherArguments = list<ClauseArgument>{stmtArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg};

	SECTION("Not equal, different types") {
		for (ClauseArgument argument: otherArguments) {
			RelationshipClause* modifiesPClauseDiffLhsArg = new ModifiesPClause(firstProcArg, argument);
			testEquals(modifiesPClauseDiffLhsArg, false);

			RelationshipClause* modifiesPClauseDiffRhsArg = new ModifiesPClause(argument, firstVarArg);
			testEquals(modifiesPClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a ModifiesPClause") {
		RelationshipClause* modifiesSClause = new ModifiesSClause(stmtArg, firstVarArg);
		testEquals(modifiesSClause, false);

		// TODO: Add more when other Clause classes are implemented
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("ModifiesPClause: test execute") {}
