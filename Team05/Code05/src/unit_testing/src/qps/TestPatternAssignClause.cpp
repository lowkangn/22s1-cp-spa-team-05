#include "catch.hpp"
#include <qps/query/clause/PatternAssignClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("PatternAssignClause: test equals") {
	auto testEquals = [](shared_ptr<PatternClause> other, bool expected) {
		// given
		ClauseArgument patternArg = ClauseArgument::createAssignArg("a");
		ClauseArgument lhs = ClauseArgument::createVariableArg("v");
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		PatternAssignClause patternAssignClause = PatternAssignClause(patternArg, lhs, rhs);

		// when
		bool actual = patternAssignClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstPatternArg = ClauseArgument::createAssignArg("a");
	ClauseArgument secondPatternArg = ClauseArgument::createAssignArg("a1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");
	ClauseArgument wildCardArg = ClauseArgument::createWildcardArg();

	ClauseArgument stringLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument patternStringArg = ClauseArgument::createPatternStringArg("x+3");
	ClauseArgument patternStringWithWildcardsArg = ClauseArgument::createPatternStringWithWildcardsArg("x+3");

	SECTION("Equal") {
		shared_ptr<PatternClause> patternAssignClauseAgain(new PatternAssignClause(firstPatternArg, firstVarArg, wildCardArg));
		testEquals(patternAssignClauseAgain, true);
	}

	SECTION("Different first arg") {
		shared_ptr<PatternClause> patternAssignClauseDiffFirstArg(new PatternAssignClause(secondPatternArg, firstVarArg, wildCardArg));
		testEquals(patternAssignClauseDiffFirstArg, false);
	}

	SECTION("Different second arg") {
		shared_ptr<PatternClause> patternAssignClauseDiffSecondArg(new PatternAssignClause(firstPatternArg, secondVarArg, wildCardArg));
		testEquals(patternAssignClauseDiffSecondArg, false);

		patternAssignClauseDiffSecondArg = shared_ptr<PatternClause>(new PatternAssignClause(firstPatternArg, stringLiteralArg, wildCardArg));
		testEquals(patternAssignClauseDiffSecondArg, false);

		patternAssignClauseDiffSecondArg = shared_ptr<PatternClause>(new PatternAssignClause(firstPatternArg, wildCardArg, wildCardArg));
		testEquals(patternAssignClauseDiffSecondArg, false);
	}

	SECTION("Different third arg") {
		shared_ptr<PatternClause> patternAssignClauseDiffThirdArg(new PatternAssignClause(firstPatternArg, firstVarArg, stringLiteralArg));
		testEquals(patternAssignClauseDiffThirdArg, false);

		patternAssignClauseDiffThirdArg = shared_ptr<PatternClause>(new PatternAssignClause(firstPatternArg, firstVarArg, patternStringArg));
		testEquals(patternAssignClauseDiffThirdArg, false);

		patternAssignClauseDiffThirdArg = shared_ptr<PatternClause>(new PatternAssignClause(firstPatternArg, firstVarArg, patternStringWithWildcardsArg));
		testEquals(patternAssignClauseDiffThirdArg, false);
	}


}

TEST_CASE("PatternAssignClause: test execute") {

}