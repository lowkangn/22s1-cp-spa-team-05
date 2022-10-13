#include "catch.hpp"
#include <qps/query/clause/PatternIfClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("PatternifClause: test equals") {
	auto testEquals = [](shared_ptr<PatternClause> other, bool expected) {
		// given
		ClauseArgument patternArg = ClauseArgument::createIfArg("i");
		ClauseArgument lhs = ClauseArgument::createVariableArg("v");
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		PatternIfClause patternIfClause = PatternIfClause(patternArg, lhs, rhs);

		// when
		bool actual = patternIfClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstPatternArg = ClauseArgument::createIfArg("i");
	ClauseArgument secondPatternArg = ClauseArgument::createIfArg("w1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");
	ClauseArgument wildCardArg = ClauseArgument::createWildcardArg();

	ClauseArgument stringLiteralArg = ClauseArgument::createStringLiteralArg("x");

	SECTION("Equal") {
		shared_ptr<PatternClause> patternIfClauseAgain(new PatternIfClause(firstPatternArg, firstVarArg, wildCardArg));
		testEquals(patternIfClauseAgain, true);
	}

	SECTION("Different first arg") {
		shared_ptr<PatternClause> patternifClauseDiffFirstArg(new PatternIfClause(secondPatternArg, firstVarArg, wildCardArg));
		testEquals(patternifClauseDiffFirstArg, false);
	}

	SECTION("Different second arg") {
		shared_ptr<PatternClause> patternIfClauseDiffSecondArg(new PatternIfClause(firstPatternArg, secondVarArg, wildCardArg));
		testEquals(patternIfClauseDiffSecondArg, false);

		patternIfClauseDiffSecondArg = shared_ptr<PatternClause>(new PatternIfClause(firstPatternArg, stringLiteralArg, wildCardArg));
		testEquals(patternIfClauseDiffSecondArg, false);

		patternIfClauseDiffSecondArg = shared_ptr<PatternClause>(new PatternIfClause(firstPatternArg, wildCardArg, wildCardArg));
		testEquals(patternIfClauseDiffSecondArg, false);
	}

	// Note: Third arg testing not necessary since third arg (second entry in bracket) is wildcard
}


// =============== INTEGRATION TESTS ====================
// this prevents other files from using these variables
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
	1:		if (x > 0 && y == 0 || z != 1) {
	2:			i = 1;
			}
	3:		if (a < 4 && b == 2) {
	4:			j = 1;
			}
	5:		if (b < 4 && d == 2) {
	6:			j = 1;
			}
		}
	*/

	// Initialise patterns
	Pattern x1Pattern = Pattern::createIfPattern(1, "x");
	Pattern y1Pattern = Pattern::createIfPattern(1, "y");
	Pattern z1Pattern = Pattern::createIfPattern(1, "z");
	Pattern a3Pattern = Pattern::createIfPattern(3, "a");
	Pattern b3Pattern = Pattern::createIfPattern(3, "b");
	Pattern b5Pattern = Pattern::createIfPattern(5, "b");
	Pattern d5Pattern = Pattern::createIfPattern(5, "d");

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
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
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

TEST_CASE("PatternifClause: test execute") {
	auto testExecute = [](PatternIfClause patternIfClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = patternIfClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};
	// ------ PKB ------
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Pattern> patterns{ x1Pattern, y1Pattern, z1Pattern, a3Pattern, b3Pattern, b5Pattern, d5Pattern };

	pkb->addPatterns(patterns);

	// ------ QPS ------
	PatternIfClause clause = PatternIfClause(ifArg, varArg, wildcardArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { firstRelationship, secondRelationship, thirdRelationship,
														fourthRelationship, fifthRelationship, sixthRelationship, seventhRelationship };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(ifArg, varArg, expectedRetrievedFromPkb);

	SECTION("LHS variable, RHS wildcard - return all results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS wildcard, RHS wildcard - return all results") {
		clause = PatternIfClause(ifArg, wildcardArg, wildcardArg);
		expectedClauseResult = RelationshipClauseResult(ifArg, wildcardArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal, one result") {
		clause = PatternIfClause(ifArg, firstStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { firstRelationship };
		expectedClauseResult = RelationshipClauseResult(ifArg, firstStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternIfClause(ifArg, fourthStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { fourthRelationship };
		expectedClauseResult = RelationshipClauseResult(ifArg, fourthStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal, two results") {
		clause = PatternIfClause(ifArg, fifthStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { fifthRelationship, seventhRelationship };
		expectedClauseResult = RelationshipClauseResult(ifArg, fifthStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal - empty results") {
		clause = PatternIfClause(ifArg, seventhStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { };
		expectedClauseResult = RelationshipClauseResult(ifArg, seventhStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}