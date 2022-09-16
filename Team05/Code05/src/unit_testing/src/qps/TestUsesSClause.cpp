#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("UsesSClause: test equals") {
	auto testEquals = [](const RelationshipClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		UsesSClause usesSClause = UsesSClause(lhs, rhs);

		// when
		bool actual = usesSClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");

	SECTION("Equal") {
		RelationshipClause* usesSClauseAgain = new UsesSClause(firstStmtArg, firstVarArg);
		testEquals(usesSClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		RelationshipClause* usesSClauseDiffLhsString = new UsesSClause(secondStmtArg, firstVarArg);
		testEquals(usesSClauseDiffLhsString, false);

		RelationshipClause* usesSClauseDiffRhsString = new UsesSClause(firstStmtArg, secondVarArg);
		testEquals(usesSClauseDiffRhsString, false);

		RelationshipClause* usesSClauseDiffStrings = new UsesSClause(secondStmtArg, secondVarArg);
		testEquals(usesSClauseDiffStrings, false);
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

	list<ClauseArgument> otherArguments = list<ClauseArgument>{ procArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg };

	SECTION("Not equal, different types") {
		for (ClauseArgument argument : otherArguments) {
			RelationshipClause* usesSClauseDiffLhsArg = new UsesSClause(firstStmtArg, argument);
			testEquals(usesSClauseDiffLhsArg, false);

			RelationshipClause* usesSClauseDiffRhsArg = new UsesSClause(argument, firstVarArg);
			testEquals(usesSClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a UsesSClause") {
		RelationshipClause* usesPClause = new UsesPClause(procArg, firstVarArg);
		testEquals(usesPClause, false);

		// TODO: Add more when other Clause classes are implemented
		RelationshipClause* modifiesPClause = new ModifiesPClause(procArg, firstVarArg);
		testEquals(modifiesPClause, false);

		RelationshipClause* modifiesSClause = new ModifiesSClause(assignArg, firstVarArg);
		testEquals(modifiesSClause, false);
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("UsesSClause: test execute") {}
