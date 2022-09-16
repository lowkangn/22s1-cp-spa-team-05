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
		ClauseArgument lhs = ClauseArgument("s", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		UsesSClause usesSClause = UsesSClause(lhs, rhs);

		// when
		bool actual = usesSClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument("s", ArgumentType::STMT);
	ClauseArgument secondStmtArg = ClauseArgument("s1", ArgumentType::STMT);
	ClauseArgument firstVarArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument secondVarArg = ClauseArgument("v1", ArgumentType::VARIABLE);

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

	ClauseArgument readArg = ClauseArgument("r", ArgumentType::READ);
	ClauseArgument printArg = ClauseArgument("p", ArgumentType::PRINT);
	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
	ClauseArgument callArg = ClauseArgument("c", ArgumentType::CALL);
	ClauseArgument whileArg = ClauseArgument("w", ArgumentType::WHILE);
	ClauseArgument ifArg = ClauseArgument("i", ArgumentType::IF);
	ClauseArgument procArg = ClauseArgument("p", ArgumentType::PROCEDURE);
	ClauseArgument constArg = ClauseArgument("c", ArgumentType::CONSTANT);
	ClauseArgument lineNumArg = ClauseArgument("1", ArgumentType::LINE_NUMBER);
	ClauseArgument stringLitArg = ClauseArgument("x", ArgumentType::STRING_LITERAL);
	ClauseArgument wildcardArg = ClauseArgument("_", ArgumentType::WILDCARD);

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
