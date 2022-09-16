#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesPClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		ModifiesPClause modifiesPClause = ModifiesPClause(lhs, rhs);

		// when
		bool actual = modifiesPClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument secondProcArg = ClauseArgument::createProcedureArg("sp1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");

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

	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument callArg = ClauseArgument::createCallArg("c");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

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
