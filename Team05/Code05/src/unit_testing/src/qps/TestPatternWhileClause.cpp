#include "catch.hpp"
#include <qps/query/clause/PatternWhileClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("PatternWhileClause: test equals") {
	auto testEquals = [](shared_ptr<PatternClause> other, bool expected) {
		// given
		ClauseArgument patternArg = ClauseArgument::createWhileArg("w");
		ClauseArgument lhs = ClauseArgument::createVariableArg("v");
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		PatternWhileClause patternWhileClause = PatternWhileClause(patternArg, lhs, rhs);

		// when
		bool actual = patternWhileClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstPatternArg = ClauseArgument::createWhileArg("w");
	ClauseArgument secondPatternArg = ClauseArgument::createWhileArg("w1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");
	ClauseArgument wildCardArg = ClauseArgument::createWildcardArg();

	ClauseArgument stringLiteralArg = ClauseArgument::createStringLiteralArg("x");
	
	SECTION("Equal") {
		shared_ptr<PatternClause> patternWhileClauseAgain(new PatternWhileClause(firstPatternArg, firstVarArg, wildCardArg));
		testEquals(patternWhileClauseAgain, true);
	}

	SECTION("Different first arg") {
		shared_ptr<PatternClause> patternWhileClauseDiffFirstArg(new PatternWhileClause(secondPatternArg, firstVarArg, wildCardArg));
		testEquals(patternWhileClauseDiffFirstArg, false);
	}

	SECTION("Different second arg") {
		shared_ptr<PatternClause> patternWhileClauseDiffSecondArg(new PatternWhileClause(firstPatternArg, secondVarArg, wildCardArg));
		testEquals(patternWhileClauseDiffSecondArg, false);

		patternWhileClauseDiffSecondArg = shared_ptr<PatternClause>(new PatternWhileClause(firstPatternArg, stringLiteralArg, wildCardArg));
		testEquals(patternWhileClauseDiffSecondArg, false);

		patternWhileClauseDiffSecondArg = shared_ptr<PatternClause>(new PatternWhileClause(firstPatternArg, wildCardArg, wildCardArg));
		testEquals(patternWhileClauseDiffSecondArg, false);
	}

	// Note: Third arg testing not necessary since third arg (second entry in bracket) is wildcard
}


// =============== INTEGRATION TESTS ====================
// this prevents other files from using these variables
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
	1:		while (x > 0 && y == 0 || z != 1) {
	2:			i = 1;
			}
	3:		while (a < 4 && b == 2) {
	4:			j = 1;
			}
	5:		while (b < 4 && d == 2) {
	6:			j = 1;
			}
		}
	*/

	// Initialise patterns
	Pattern x1Pattern = Pattern::createWhilePattern(1, "x");
	Pattern y1Pattern = Pattern::createWhilePattern(1, "y");
	Pattern z1Pattern = Pattern::createWhilePattern(1, "z");
	Pattern a3Pattern = Pattern::createWhilePattern(3, "a");
	Pattern b3Pattern = Pattern::createWhilePattern(3, "b");
	Pattern b5Pattern = Pattern::createWhilePattern(5, "b");
	Pattern d5Pattern = Pattern::createWhilePattern(5, "d");

	// Initialise PQLEntities and PQLRelationships
	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(3);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(5);
	PQLEntity firstVarEntity = PQLEntity::generateVariable("x");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("y");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("z");
	PQLEntity fourthVarEntity = PQLEntity::generateVariable("a");
	PQLEntity fifthVarEntity = PQLEntity::generateVariable("b");
	PQLEntity sixthVarEntity = PQLEntity::generateVariable("d");

	PQLRelationship firstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
	PQLRelationship secondRelationship = PQLRelationship(firstStmtEntity, secondVarEntity);
	PQLRelationship thirdRelationship = PQLRelationship(firstStmtEntity, thirdVarEntity);
	PQLRelationship fourthRelationship = PQLRelationship(secondStmtEntity, fourthVarEntity);
	PQLRelationship fifthRelationship = PQLRelationship(secondStmtEntity, fifthVarEntity);
	PQLRelationship sixthRelationship = PQLRelationship(thirdStmtEntity, sixthVarEntity);
	PQLRelationship seventhRelationship = PQLRelationship(thirdStmtEntity, fifthVarEntity);

	// Initialise ClauseArguments
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
	ClauseArgument anotherWildcardArg = ClauseArgument::createWildcardArg();
	ClauseArgument firstStringLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument secondStringLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument thirdStringLiteralArg = ClauseArgument::createStringLiteralArg("z");
	ClauseArgument fourthStringLiteralArg = ClauseArgument::createStringLiteralArg("a");
	ClauseArgument fifthStringLiteralArg = ClauseArgument::createStringLiteralArg("b");
	ClauseArgument sixthStringLiteralArg = ClauseArgument::createStringLiteralArg("d");
	ClauseArgument seventhStringLiteralArg = ClauseArgument::createStringLiteralArg("e");


};

TEST_CASE("PatternWhileClause: test execute") {
	auto testExecute = [](PatternWhileClause patternWhileClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = patternWhileClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};
	// ------ PKB ------
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Pattern> patterns{ x1Pattern, y1Pattern, z1Pattern, a3Pattern, b3Pattern, b5Pattern, d5Pattern  };

	pkb->addPatterns(patterns);

	// ------ QPS ------
	PatternWhileClause clause = PatternWhileClause(whileArg, varArg, wildcardArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { firstRelationship, secondRelationship, thirdRelationship,
														fourthRelationship, fifthRelationship, sixthRelationship, seventhRelationship };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(whileArg, varArg, expectedRetrievedFromPkb);

	SECTION("LHS variable, RHS wildcard - return all results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS wildcard, RHS wildcard - return all results") {
		clause = PatternWhileClause(whileArg, wildcardArg, wildcardArg);
		expectedClauseResult = RelationshipClauseResult(whileArg, wildcardArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal, one result") {
		clause = PatternWhileClause(whileArg, firstStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { firstRelationship };
		expectedClauseResult = RelationshipClauseResult(whileArg, firstStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternWhileClause(whileArg, fourthStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { fourthRelationship };
		expectedClauseResult = RelationshipClauseResult(whileArg, fourthStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal, two results") {
		clause = PatternWhileClause(whileArg, fifthStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { fifthRelationship, seventhRelationship };
		expectedClauseResult = RelationshipClauseResult(whileArg, fifthStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal - empty results") {
		clause = PatternWhileClause(whileArg, seventhStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { };
		expectedClauseResult = RelationshipClauseResult(whileArg, seventhStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}