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
		ClauseArgument lhs = ClauseArgument("p", ArgumentType::PROCEDURE);
		ClauseArgument rhs = ClauseArgument("v", ArgumentType::VARIABLE);
		UsesPClause usesPClause = UsesPClause(lhs, rhs);

		// when
		bool actual = usesPClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcArg = ClauseArgument("p", ArgumentType::PROCEDURE);
	ClauseArgument secondProcArg = ClauseArgument("sp1", ArgumentType::PROCEDURE);
	ClauseArgument firstVarArg = ClauseArgument("v", ArgumentType::VARIABLE);
	ClauseArgument secondVarArg = ClauseArgument("v1", ArgumentType::VARIABLE);

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
