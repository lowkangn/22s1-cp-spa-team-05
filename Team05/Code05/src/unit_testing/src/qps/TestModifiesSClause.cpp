#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesSClause: test equals") {
	auto testEquals = [](const SuchThatClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument("s", ArgumentType::STMT);
		ClauseArgument rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		ModifiesSClause modifiesSClause = ModifiesSClause(lhs, rhs);

		// when
		bool actual = modifiesSClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument("s", ArgumentType::STMT);
	ClauseArgument secondStmtArg = ClauseArgument("s1", ArgumentType::STMT);
	ClauseArgument firstVarArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument secondVarArg = ClauseArgument("v1", ArgumentType::VARIABLE);

	SECTION("Equal") {
		SuchThatClause* modifiesSClauseAgain = new ModifiesSClause(firstStmtArg, firstVarArg);
		testEquals(modifiesSClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		SuchThatClause* modifiesSClauseDiffLhsString = new ModifiesSClause(secondStmtArg, firstVarArg);
		testEquals(modifiesSClauseDiffLhsString, false);

		SuchThatClause* modifiesSClauseDiffRhsString = new ModifiesSClause(firstStmtArg, secondVarArg);
		testEquals(modifiesSClauseDiffRhsString, false);

		SuchThatClause* modifiesSClauseDiffStrings = new ModifiesSClause(secondStmtArg, secondVarArg);
		testEquals(modifiesSClauseDiffStrings, false);
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

	list<ClauseArgument> otherArguments = list<ClauseArgument>{procArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg};

	SECTION("Not equal, different types") {
		for (ClauseArgument argument: otherArguments) {
			SuchThatClause* modifiesSClauseDiffLhsArg = new ModifiesSClause(firstStmtArg, argument);
			testEquals(modifiesSClauseDiffLhsArg, false);

			SuchThatClause* modifiesSClauseDiffRhsArg = new ModifiesSClause(argument, firstVarArg);
			testEquals(modifiesSClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a ModifiesSClause") {
		SuchThatClause* modifiesPClause = new ModifiesPClause(procArg, firstVarArg);
		testEquals(modifiesPClause, false);

		// TODO: Add more when other Clause classes are implemented
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("ModifiesSClause: test execute") {}
