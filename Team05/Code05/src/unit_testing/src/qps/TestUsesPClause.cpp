#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("UsesPClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		UsesPClause usesPClause = UsesPClause(lhs, rhs);

		// when
		bool actual = usesPClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument secondProcArg = ClauseArgument::createProcedureArg("sp1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");

	SECTION("Equal") {
		RelationshipClause* usesPClauseAgain = new UsesPClause(firstProcArg, firstVarArg);
		testEquals(usesPClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		RelationshipClause* usesPClauseDiffLhsString = new UsesPClause(secondProcArg, firstVarArg);
		testEquals(usesPClauseDiffLhsString, false);

		RelationshipClause* usesPClauseDiffRhsString = new UsesPClause(firstProcArg, secondVarArg);
		testEquals(usesPClauseDiffRhsString, false);

		RelationshipClause* usesPClauseDiffStrings = new UsesPClause(secondProcArg, secondVarArg);
		testEquals(usesPClauseDiffStrings, false);
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

	list<ClauseArgument> otherArguments = list<ClauseArgument>{ stmtArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg };

	SECTION("Not equal, different types") {
		for (ClauseArgument argument : otherArguments) {
			RelationshipClause* usesPClauseDiffLhsArg = new UsesPClause(firstProcArg, argument);
			testEquals(usesPClauseDiffLhsArg, false);

			RelationshipClause* usesPClauseDiffRhsArg = new UsesPClause(argument, firstVarArg);
			testEquals(usesPClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a UsesPClause") {
		RelationshipClause* usesSClause = new UsesSClause(stmtArg, firstVarArg);
		testEquals(usesSClause, false);

		// TODO: Add more when other Clause classes are implemented
		RelationshipClause* modifiesPClause = new ModifiesPClause(firstProcArg, firstVarArg);
		testEquals(modifiesPClause, false);

		RelationshipClause* modifiesSClause = new ModifiesSClause(assignArg, firstVarArg);
		testEquals(modifiesSClause, false);
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("UsesPClause: test execute") {}
