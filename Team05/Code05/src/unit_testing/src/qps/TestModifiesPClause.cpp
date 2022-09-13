#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesPClause: test equals") {
	auto testEquals = [](const SuchThatClause* other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument("p", ArgumentType::PROCEDURE);
		ClauseArgument rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		ModifiesPClause modifiesPClause = ModifiesPClause(lhs, rhs);

		// when
		bool actual = modifiesPClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcArg = ClauseArgument("p", ArgumentType::PROCEDURE);
	ClauseArgument secondProcArg = ClauseArgument("sp1", ArgumentType::PROCEDURE);
	ClauseArgument firstVarArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument secondVarArg = ClauseArgument("v1", ArgumentType::VARIABLE);

	SECTION("Equal") {
		SuchThatClause* modifiesPClauseAgain = new ModifiesPClause(firstProcArg, firstVarArg);
		testEquals(modifiesPClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		SuchThatClause* modifiesPClauseDiffLhsString = new ModifiesPClause(secondProcArg, firstVarArg);
		testEquals(modifiesPClauseDiffLhsString, false);

		SuchThatClause* modifiesPClauseDiffRhsString = new ModifiesPClause(firstProcArg, secondVarArg);
		testEquals(modifiesPClauseDiffRhsString, false);

		SuchThatClause* modifiesPClauseDiffStrings = new ModifiesPClause(secondProcArg, secondVarArg);
		testEquals(modifiesPClauseDiffStrings, false);
	}

	ClauseArgument stmtArg = ClauseArgument("s", ArgumentType::STMT);
	ClauseArgument readArg = ClauseArgument("r", ArgumentType::READ);
	ClauseArgument printArg = ClauseArgument("p", ArgumentType::PRINT);
	ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
	ClauseArgument callArg = ClauseArgument("c", ArgumentType::CALL);
	ClauseArgument whileArg = ClauseArgument("w", ArgumentType::WHILE);
	ClauseArgument ifArg = ClauseArgument("i", ArgumentType::IF);
	ClauseArgument constArg = ClauseArgument("c", ArgumentType::CONSTANT);
	ClauseArgument lineNumArg = ClauseArgument("1", ArgumentType::LINE_NUMBER);
	ClauseArgument stringLitArg = ClauseArgument("x", ArgumentType::STRING_LITERAL);
	ClauseArgument wildcardArg = ClauseArgument("_", ArgumentType::WILDCARD);

	list<ClauseArgument> otherArguments = list<ClauseArgument>{stmtArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg};

	SECTION("Not equal, different types") {
		for (ClauseArgument argument: otherArguments) {
			SuchThatClause* modifiesPClauseDiffLhsArg = new ModifiesPClause(firstProcArg, argument);
			testEquals(modifiesPClauseDiffLhsArg, false);

			SuchThatClause* modifiesPClauseDiffRhsArg = new ModifiesPClause(argument, firstVarArg);
			testEquals(modifiesPClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a ModifiesPClause") {
		SuchThatClause* modifiesSClause = new ModifiesSClause(stmtArg, firstVarArg);
		testEquals(modifiesSClause, false);

		// TODO: Add more when other Clause classes are implemented
	}
}

// =============== INTEGRATION TESTS ====================

// TODO: Requires PKB interaction implementation
TEST_CASE("ModifiesPClause: test execute") {}
