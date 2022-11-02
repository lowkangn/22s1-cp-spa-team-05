#include "catch.hpp"
#include <qps/query_evaluator/QueryResultsCombiner.h>

using namespace std;

// ==================== UNIT TESTS ====================

namespace {
	PQLEntity pqlR1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlP4 = PQLEntity::generateStatement(4);
	PQLEntity pqlI5 = PQLEntity::generateStatement(5);
	PQLEntity pqlA6 = PQLEntity::generateStatement(6);
	PQLEntity pqlI7 = PQLEntity::generateStatement(7);
	PQLEntity pqlA8 = PQLEntity::generateStatement(8);
	PQLEntity pqlA9 = PQLEntity::generateStatement(9);
	PQLEntity pqlC10 = PQLEntity::generateStatement(10);
	PQLEntity pqlC11 = PQLEntity::generateStatement(11);

	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlProcX = PQLEntity::generateProcedure("x");
	PQLEntity pqlProcY = PQLEntity::generateProcedure("y");
	PQLEntity pqlX = PQLEntity::generateVariable("x");
	PQLEntity pqlY = PQLEntity::generateVariable("y");
	PQLEntity pqlZ = PQLEntity::generateVariable("z");
	PQLEntity pql0 = PQLEntity::generateConstant(0);
	PQLEntity pql1 = PQLEntity::generateConstant(1);
	PQLEntity pql3 = PQLEntity::generateConstant(3);
	PQLEntity pql5 = PQLEntity::generateConstant(5);

	PQLRelationship pqlFollowsR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsP4I5 = PQLRelationship(pqlP4, pqlI5);
	PQLRelationship pqlFollowsW3I7 = PQLRelationship(pqlW3, pqlI7);

	PQLRelationship pqlFollowsStarR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsStarR1W3 = PQLRelationship(pqlR1, pqlW3);
	PQLRelationship pqlFollowsStarR1I7 = PQLRelationship(pqlR1, pqlI7);

	PQLRelationship pqlFollowsStarA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsStarA2I7 = PQLRelationship(pqlA2, pqlI7);

	PQLRelationship pqlFollowsStarP4I5 = PQLRelationship(pqlP4, pqlI5);

	PQLRelationship pqlFollowsStarW3I7 = PQLRelationship(pqlW3, pqlI7);

	PQLRelationship pqlParentW3P4 = PQLRelationship(pqlW3, pqlP4);
	PQLRelationship pqlParentW3I5 = PQLRelationship(pqlW3, pqlI5);
	PQLRelationship pqlParentI5A6 = PQLRelationship(pqlI5, pqlA6);
	PQLRelationship pqlParentI7A8 = PQLRelationship(pqlI7, pqlA8);
	PQLRelationship pqlParentI7A9 = PQLRelationship(pqlI7, pqlA9);

	PQLRelationship pqlParentStarW3P4 = PQLRelationship(pqlW3, pqlP4);
	PQLRelationship pqlParentStarW3I5 = PQLRelationship(pqlW3, pqlI5);
	PQLRelationship pqlParentStarW3A6 = PQLRelationship(pqlW3, pqlA6);

	PQLRelationship pqlParentStarI5A6 = PQLRelationship(pqlI5, pqlA6);

	PQLRelationship pqlParentStarI7A8 = PQLRelationship(pqlI7, pqlA8);
	PQLRelationship pqlParentStarI7A9 = PQLRelationship(pqlI7, pqlA9);

	PQLRelationship pqlModifiesSR1Y = PQLRelationship(pqlR1, pqlY);
	PQLRelationship pqlModifiesSA2X = PQLRelationship(pqlA2, pqlX);
	PQLRelationship pqlModifiesSA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlModifiesSA8Y = PQLRelationship(pqlA8, pqlY);
	PQLRelationship pqlModifiesSA9Y = PQLRelationship(pqlA9, pqlY);

	PQLRelationship pqlModifiesSW3X = PQLRelationship(pqlW3, pqlX);
	PQLRelationship pqlModifiesSI5X = PQLRelationship(pqlI5, pqlX);
	PQLRelationship pqlModifiesSI7Y = PQLRelationship(pqlA9, pqlY);

	PQLRelationship pqlModifiesPMainX = PQLRelationship(pqlMain, pqlX);
	PQLRelationship pqlModifiesPMainY = PQLRelationship(pqlMain, pqlY);

	PQLRelationship pqlModifiesPXX = PQLRelationship(pqlProcX, pqlX);
	PQLRelationship pqlModifiesPXY = PQLRelationship(pqlProcX, pqlY);

	PQLRelationship pqlModifiesPYX = PQLRelationship(pqlProcY, pqlX);
	PQLRelationship pqlModifiesPYY = PQLRelationship(pqlProcY, pqlY);

	PQLRelationship pqlUsesSA2Y = PQLRelationship(pqlA2, pqlY);
	PQLRelationship pqlUsesSP4Z = PQLRelationship(pqlP4, pqlZ);
	PQLRelationship pqlUsesSA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlUsesSA9Z = PQLRelationship(pqlA9, pqlZ);

	PQLRelationship pqlUsesSW3X = PQLRelationship(pqlW3, pqlX);
	PQLRelationship pqlUsesSI5Y = PQLRelationship(pqlI5, pqlY);
	PQLRelationship pqlUsesSI5X = PQLRelationship(pqlI5, pqlX);
	PQLRelationship pqlUsesSI7Y = PQLRelationship(pqlI7, pqlY);

	PQLRelationship pqlUsesPMainY = PQLRelationship(pqlMain, pqlY);
	PQLRelationship pqlUsesPMainX = PQLRelationship(pqlMain, pqlX);
	PQLRelationship pqlUsesPMainZ = PQLRelationship(pqlMain, pqlZ);

	PQLRelationship pqlUsesPXY = PQLRelationship(pqlProcX, pqlY);
	PQLRelationship pqlUsesPXX = PQLRelationship(pqlProcX, pqlX);
	PQLRelationship pqlUsesPXZ = PQLRelationship(pqlProcX, pqlZ);

	PQLRelationship pqlUsesPYY = PQLRelationship(pqlProcY, pqlY);
	PQLRelationship pqlUsesPYX = PQLRelationship(pqlProcY, pqlX);
	PQLRelationship pqlUsesPYZ = PQLRelationship(pqlProcY, pqlZ);

	PQLRelationship pqlCallsC10PMain = PQLRelationship(pqlC10, pqlMain);
	PQLRelationship pqlCallsC11PX = PQLRelationship(pqlC11, pqlProcX);

	PQLRelationship pqlPatternA2X = PQLRelationship(pqlA2, pqlX);
	PQLRelationship pqlPatternA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlPatternA8Y = PQLRelationship(pqlA8, pqlY);
	PQLRelationship pqlPatternA9Y = PQLRelationship(pqlA9, pqlY);

	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument assign2Arg = ClauseArgument::createAssignArg("a1");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument callArg = ClauseArgument::createCallArg("ca");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument var2Arg = ClauseArgument::createVariableArg("v1");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
	ClauseArgument XStringLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument ZStringLiteralArg = ClauseArgument::createStringLiteralArg("z");
	ClauseArgument callProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(callArg);
	ClauseArgument variableVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(varArg);
	ClauseArgument lineNumber1Arg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineNumber2Arg = ClauseArgument::createLineNumberArg("2");

	Table selectStmtResultTable = {{pqlR1}, {pqlA2}, {pqlW3}, {pqlP4}, {pqlI5}, {pqlA6}, {pqlI7}, {pqlA8}, {pqlA9}, {pqlC10}, {pqlC11}};
	Table selectAssignResultTable = {{pqlA2}, {pqlA6}, {pqlA8}, {pqlA9}};
	Table selectReadResultTable = {{pqlR1}};
	Table selectWhileResultTable = {{pqlW3}};
	Table selectIfResultTable = {{pqlI5}, {pqlI7}};
	Table selectCallResultTable = {{pqlC10}, {pqlC11}};
	Table selectPrintResultTable = {{pqlP4}};
	Table selectProcResultTable = {{pqlMain}, {pqlProcX}, {pqlProcY}};
	Table selectVarResultTable = {{pqlX}, {pqlY}, {pqlZ}};
	Table selectConstResultTable = {{pql0}, {pql1}, {pql3}, {pql5}};
	Table callProcNameVarVarNameResultTable = {{pqlC11, pqlProcX, pqlX}};
	Table callXEqualsXResultTable = {{PQLEntity::generateConstant(0)}};

	ClauseResult selectStmtResult = ClauseResult({stmtArg}, selectStmtResultTable);
	ClauseResult selectAssignResult = ClauseResult({assignArg}, selectAssignResultTable);
	ClauseResult selectReadResult = ClauseResult({readArg}, selectReadResultTable);
	ClauseResult selectWhileResult = ClauseResult({whileArg}, selectWhileResultTable);
	ClauseResult selectIfResult = ClauseResult({ifArg}, selectIfResultTable);
	ClauseResult selectCallResult = ClauseResult({callArg}, selectCallResultTable);
	ClauseResult selectPrintResult = ClauseResult({printArg}, selectPrintResultTable);
	ClauseResult selectProcResult = ClauseResult({procArg}, selectProcResultTable);
	ClauseResult selectVarResult = ClauseResult({varArg}, selectVarResultTable);
	ClauseResult selectConstResult = ClauseResult({constArg}, selectConstResultTable);

	ClauseResult followsReadAssignResult = ClauseResult({readArg, assignArg}, {pqlFollowsR1A2});
	ClauseResult modifiesAssignVarResult = ClauseResult({assignArg, varArg},{pqlModifiesSA2X, pqlModifiesSA6X, pqlModifiesSA8Y, pqlModifiesSA9Y});
	ClauseResult modifiesAssignZResult = ClauseResult({assignArg, ZStringLiteralArg},Table{});
	ClauseResult parentStarWhileAssign2Result = ClauseResult({whileArg, assign2Arg}, {pqlParentStarW3A6});
	ClauseResult patternAssign2XResult = ClauseResult({assign2Arg, XStringLiteralArg}, {pqlPatternA2X, pqlPatternA6X});
	ClauseResult withCallProcNameVar2VarNameResult = ClauseResult({callArg, callProcNameAttributeArg, var2Arg, variableVarNameAttributeArg}, callProcNameVarVarNameResultTable);
	ClauseResult withXEqualsXResult = ClauseResult({wildcardArg}, callXEqualsXResultTable);
	ClauseResult withOneEqualsTwoResult = ClauseResult({lineNumber1Arg, lineNumber2Arg}, Table{});

	ClauseResult selectBooleanPlaceholderResult = ClauseResult({ClauseArgument::createWildcardArg()}, Table{{PQLEntity::generateConstant(0)}});
}

TEST_CASE("QueryResultsCombiner: test combineAll") {
	auto testCombine = [](list<ClauseResult> selectResults,
			vector<vector<vector<ClauseResult>>> optimisedConstraintResults, ClauseResult expected) {
		// given
		QueryResultsCombiner combiner = QueryResultsCombiner(selectResults, optimisedConstraintResults);

		// when
		ClauseResult actual = combiner.combineAll();

		// then

		REQUIRE(actual == expected);
	};

	list<ClauseResult> selectResults;
	vector<vector<vector<ClauseResult>>> optimisedConstraintResults;
	Table expectedTable;
	ClauseResult expected;

	SECTION("Select results only") {
		// Single result
		selectResults = {selectStmtResult};
		optimisedConstraintResults = {{}, {}};
		expected = selectStmtResult;
		testCombine(selectResults, optimisedConstraintResults, expected);

		// Multiple results
		selectResults = {selectAssignResult, selectVarResult};
		expectedTable = {
				{pqlA2, pqlX}, {pqlA2, pqlY}, {pqlA2, pqlZ},
				{pqlA6, pqlX}, {pqlA6, pqlY}, {pqlA6, pqlZ},
				{pqlA8, pqlX}, {pqlA8, pqlY}, {pqlA8, pqlZ},
				{pqlA9, pqlX}, {pqlA9, pqlY}, {pqlA9, pqlZ}
		};
		expected = ClauseResult({assignArg, varArg}, expectedTable);
		testCombine(selectResults, optimisedConstraintResults, expected);

		selectResults = {selectReadResult, selectPrintResult, selectIfResult, selectWhileResult};
		expectedTable = {
				{pqlR1, pqlP4, pqlI5, pqlW3},
				{pqlR1, pqlP4, pqlI7, pqlW3}
		};
		expected = ClauseResult({readArg, printArg, ifArg, whileArg}, expectedTable);
		testCombine(selectResults, optimisedConstraintResults, expected);
	}

	SECTION("All groups have some result with selected arg") {
		// One group - Select v such that Follows(r, a) such that Modifies(a, v)
		selectResults = {selectVarResult};
		optimisedConstraintResults = {{{followsReadAssignResult, modifiesAssignVarResult}}, {}};
		expectedTable = {
				{pqlX}
		};
		expected = ClauseResult({varArg}, expectedTable);
		testCombine(selectResults, optimisedConstraintResults, expected);

		// Multiple groups - Select <a, w> such that Follows(r, a) such that Modifies(a, v) such that Parent*(w, a1) pattern a1("x", _"x"_)
		selectResults = {selectAssignResult, selectWhileResult};
		optimisedConstraintResults = {{{followsReadAssignResult, modifiesAssignVarResult}, {parentStarWhileAssign2Result, patternAssign2XResult}}, {}};
		expectedTable = {
				{pqlA2, pqlW3}
		};
		expected = ClauseResult({assignArg, whileArg}, expectedTable);
		testCombine(selectResults, optimisedConstraintResults, expected);
	}

	SECTION("No groups have any result with selected arg") {
		// Select w such that Follows(r, a) such that Modifies(a, v)
		selectResults = {selectWhileResult};
		optimisedConstraintResults = {{}, {{followsReadAssignResult, modifiesAssignVarResult}}};
		expectedTable = {
				{pqlW3}
		};
		expected = ClauseResult({whileArg}, expectedTable);
		testCombine(selectResults, optimisedConstraintResults, expected);

		// Select <p, c> such that Follows(r, a) such that Modifies(a, v) such that Parent*(w, a2) pattern a2("x", _"x"_)
		selectResults = {selectPrintResult, selectCallResult};
		optimisedConstraintResults = {{}, {{followsReadAssignResult, modifiesAssignVarResult}, {parentStarWhileAssign2Result, patternAssign2XResult}}};
		expectedTable = {
				{pqlP4, pqlC10},
				{pqlP4, pqlC11}
		};
		expected = ClauseResult({printArg, callArg}, expectedTable);
		testCombine(selectResults, optimisedConstraintResults, expected);

		// Select BOOLEAN such that Follows(r, a) such that Modifies(a, v)
		selectResults = {};
		optimisedConstraintResults = {{}, {{followsReadAssignResult, modifiesAssignVarResult}}};
		expected = selectBooleanPlaceholderResult;
		testCombine(selectResults, optimisedConstraintResults, expected);

		// Select BOOLEAN such that Follows(r, a) such that Modifies(a, "z")
		selectResults = {};
		optimisedConstraintResults = {{}, {{followsReadAssignResult, modifiesAssignZResult}}};
		expected = {};
		testCombine(selectResults, optimisedConstraintResults, expected);

		// Select a such that Follows(r, a) such that Modifies(a, "z") such that Parent*(w, a2) pattern a2("x", _"x"_)
		selectResults = {selectAssignResult};
		optimisedConstraintResults = {{{followsReadAssignResult, modifiesAssignZResult}}, {{parentStarWhileAssign2Result, patternAssign2XResult}}};
		expected = {};
		testCombine(selectResults, optimisedConstraintResults, expected);
	}

	SECTION("Have both groups with and without selected args") {
		/* Group 1: Select v such that Follows(r, a) such that Modifies(a, v)
		*  Group 2: such that Parent*(w, a1) pattern a1("x", _"x"_)
		*  Group 3: with c.procName = v1.varName;
		*/
		selectResults = {selectVarResult};
		optimisedConstraintResults = {{{followsReadAssignResult, modifiesAssignVarResult}}, {{parentStarWhileAssign2Result, patternAssign2XResult}, {withCallProcNameVar2VarNameResult}}};
		expectedTable = {
				{pqlX}
		};
		expected = ClauseResult({varArg}, expectedTable);
		testCombine(selectResults, optimisedConstraintResults, expected);

		/* Group 1: Select v such that Follows(r, a) such that Modifies(a, v)
		*  Group 2: such that Parent*(w, a2) pattern a2("x", _"x"_)
		*  Group 3: with "x" = "x" with 1 = 2
		*/
		optimisedConstraintResults = {{{followsReadAssignResult, modifiesAssignVarResult}}, {{parentStarWhileAssign2Result, patternAssign2XResult}, {withXEqualsXResult, withOneEqualsTwoResult}}};
		expected = ClauseResult();
		testCombine(selectResults, optimisedConstraintResults, expected);
	}

}
