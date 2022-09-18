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
	ClauseArgument patternStringArg = ClauseArgument::createPatternStringArg("x3+");
	ClauseArgument patternStringWithWildcardsArg = ClauseArgument::createPatternStringWithWildcardsArg("x3+");

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

// =============== INTEGRATION TESTS ====================
// this prevents other files from using these variables
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
	 		while (x > 0) {
	1:			y = x + 1;
	2:			z = x * 3 - y / 2;
	3:			x = x - 1;
	 		}
		}
	*/

	// Initialise Patterns
	Pattern a1 = Pattern::createAssignPattern(1, "y", "x1+");
	Pattern a2 = Pattern::createAssignPattern(2, "z", "x3*y2/-");
	Pattern a3 = Pattern::createAssignPattern(3, "x", "x1-");

	// Initialise PQLEntities and PQLRelationships
	PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
	PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);
	PQLEntity thirdStmtEntity = PQLEntity::generateStatement(3);
	PQLEntity firstVarEntity = PQLEntity::generateVariable("y");
	PQLEntity secondVarEntity = PQLEntity::generateVariable("z");
	PQLEntity thirdVarEntity = PQLEntity::generateVariable("x");

	PQLRelationship firstRelationship = PQLRelationship(firstStmtEntity, firstVarEntity);
	PQLRelationship secondRelationship = PQLRelationship(secondStmtEntity, secondVarEntity);
	PQLRelationship thirdRelationship = PQLRelationship(thirdStmtEntity, thirdVarEntity);

	// Initialise ClauseArguments
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
	ClauseArgument anotherWildcardArg = ClauseArgument::createWildcardArg();
	ClauseArgument firstStringLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument secondStringLiteralArg = ClauseArgument::createStringLiteralArg("z");
	ClauseArgument thirdStringLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument nonExistentStringLiteralArg = ClauseArgument::createStringLiteralArg("aAaAaA");
	ClauseArgument firstPatternStringArg = ClauseArgument::createPatternStringArg("x1+");
	ClauseArgument secondPatternStringArg = ClauseArgument::createPatternStringArg("x3*y2/-");
	ClauseArgument thirdPatternStringArg = ClauseArgument::createPatternStringArg("x1-");
	ClauseArgument firstNonExistentPatternStringArg = ClauseArgument::createPatternStringArg("3y-");
	ClauseArgument secondNonExistentPatternStringArg = ClauseArgument::createPatternStringArg("x3*");
	ClauseArgument firstPatternStringWithWildcardsArg = ClauseArgument::createPatternStringWithWildcardsArg("_x_");
	ClauseArgument secondPatternStringWithWildcardsArg = ClauseArgument::createPatternStringWithWildcardsArg("_y_");
	ClauseArgument thirdPatternStringWithWildcardsArg = ClauseArgument::createPatternStringWithWildcardsArg("_x3*_");
	ClauseArgument fourthPatternStringWithWildcardsArg = ClauseArgument::createPatternStringWithWildcardsArg("_1_");
	ClauseArgument nonExistentPatternStringWithWildcardsArg = ClauseArgument::createPatternStringWithWildcardsArg("_3y-_");

};

TEST_CASE("PatternAssignClause: test execute") {
	auto testExecute = [](PatternAssignClause patternAssignClause,
						  RelationshipClauseResult expectedClauseResult,
						  shared_ptr<PKB> pkb) {
		// given
		shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

		// when
		shared_ptr<RelationshipClauseResult> resPtr = patternAssignClause.execute(pkbInterface);
		RelationshipClauseResult actualClauseResult = *resPtr;

		// then
		REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ PKB ------
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Pattern> patterns{ a1, a2, a3 };

	pkb->addPatterns(patterns);

	// ------ QPS ------
	PatternAssignClause clause = PatternAssignClause(assignArg, varArg, wildcardArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { firstRelationship, secondRelationship, thirdRelationship };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);

	SECTION("LHS variable, RHS wildcard - return all results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS wildcard, RHS wildcard - return all results") {
		clause = PatternAssignClause(assignArg, wildcardArg, anotherWildcardArg);
		expectedClauseResult = RelationshipClauseResult(assignArg, wildcardArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal, RHS wildcard - non empty results") {
		clause = PatternAssignClause(assignArg, firstStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { firstRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, firstStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, secondStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { secondRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, secondStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, thirdStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { thirdRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, thirdStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS string literal, RHS wildcard - empty results") {
		clause = PatternAssignClause(assignArg, nonExistentStringLiteralArg, wildcardArg);
		expectedRetrievedFromPkb = { };
		expectedClauseResult = RelationshipClauseResult(assignArg, nonExistentStringLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS variable, RHS exact string - return non-empty results") {
		clause = PatternAssignClause(assignArg, varArg, firstPatternStringArg);
		expectedRetrievedFromPkb = { firstRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, varArg, secondPatternStringArg);
		expectedRetrievedFromPkb = { secondRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, varArg, thirdPatternStringArg);
		expectedRetrievedFromPkb = { thirdRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

	}

	SECTION("LHS variable, RHS exact string - return empty results") {
		clause = PatternAssignClause(assignArg, varArg, firstNonExistentPatternStringArg);
		expectedRetrievedFromPkb = { };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, varArg, secondNonExistentPatternStringArg);
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS variable, RHS string with wildcards - return non-empty results") {
		clause = PatternAssignClause(assignArg, varArg, firstPatternStringWithWildcardsArg);
		expectedRetrievedFromPkb = { firstRelationship, secondRelationship, thirdRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, varArg, secondPatternStringWithWildcardsArg);
		expectedRetrievedFromPkb = { secondRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, varArg, thirdPatternStringWithWildcardsArg);
		expectedRetrievedFromPkb = { secondRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = PatternAssignClause(assignArg, varArg, fourthPatternStringWithWildcardsArg);
		expectedRetrievedFromPkb = { firstRelationship, thirdRelationship };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("LHS variable, RHS string with wildcards - return empty results") {
		clause = PatternAssignClause(assignArg, varArg, nonExistentPatternStringWithWildcardsArg);
		expectedRetrievedFromPkb = { };
		expectedClauseResult = RelationshipClauseResult(assignArg, varArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

}