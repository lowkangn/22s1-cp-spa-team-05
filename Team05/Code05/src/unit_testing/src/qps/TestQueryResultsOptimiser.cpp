#include "catch.hpp"
#include <qps/query_evaluator/QueryResultsOptimiser.h>

using namespace std;


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
    PQLRelationship pqlParentI7C10 = PQLRelationship(pqlI7, pqlC10);

    PQLRelationship pqlParentStarW3P4 = PQLRelationship(pqlW3, pqlP4);
    PQLRelationship pqlParentStarW3I5 = PQLRelationship(pqlW3, pqlI5);
    PQLRelationship pqlParentStarW3A6 = PQLRelationship(pqlW3, pqlA6);

    PQLRelationship pqlParentStarI5A6 = PQLRelationship(pqlI5, pqlA6);

    PQLRelationship pqlParentStarI7A8 = PQLRelationship(pqlI7, pqlA8);
    PQLRelationship pqlParentStarI7A9 = PQLRelationship(pqlI7, pqlA9);
    PQLRelationship pqlParentStarI7C10 = PQLRelationship(pqlI7, pqlC10);

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

    PQLRelationship pqlCallsPXPMain = PQLRelationship(pqlProcX, pqlMain);
    PQLRelationship pqlCallsPYPX = PQLRelationship(pqlProcY, pqlProcX);

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
    ClauseArgument proc2Arg = ClauseArgument::createProcedureArg("p2");
    ClauseArgument callArg = ClauseArgument::createCallArg("ca");
    ClauseArgument varArg = ClauseArgument::createVariableArg("v");
    ClauseArgument var2Arg = ClauseArgument::createVariableArg("v1");
    ClauseArgument constArg = ClauseArgument::createConstantArg("c");
    ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
    ClauseArgument XStringLiteralArg = ClauseArgument::createStringLiteralArg("x");
    ClauseArgument ZStringLiteralArg = ClauseArgument::createStringLiteralArg("z");
    ClauseArgument callProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(callArg);
    ClauseArgument variableVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(varArg);
    ClauseArgument constValueAttributeArg = ClauseArgument::createValueAttributeArg(constArg);
    ClauseArgument readStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(readArg);
    ClauseArgument lineNumber1Arg = ClauseArgument::createLineNumberArg("1");
    ClauseArgument lineNumber2Arg = ClauseArgument::createLineNumberArg("2");

    Table selectStmtResultTable = { {pqlR1}, {pqlA2}, {pqlW3}, {pqlP4}, {pqlI5}, {pqlA6}, {pqlI7}, {pqlA8}, {pqlA9}, {pqlC10}, {pqlC11} };
    Table selectAssignResultTable = { {pqlA2}, {pqlA6}, {pqlA8}, {pqlA9} };
    Table selectReadResultTable = { {pqlR1} };
    Table selectWhileResultTable = { {pqlW3} };
    Table selectIfResultTable = { {pqlI5}, {pqlI7} };
    Table selectCallResultTable = { {pqlC10}, {pqlC11} };
    Table selectPrintResultTable = { {pqlP4} };
    Table selectProcResultTable = { {pqlMain}, {pqlProcX}, {pqlProcY} };
    Table selectVarResultTable = { {pqlX}, {pqlY}, {pqlZ} };
    Table selectConstResultTable = { {pql0}, {pql1}, {pql3}, {pql5} };

    Table callProcNameVarVarNameResultTable = { {pqlC11, pqlProcX, pqlX} };
    Table constValueReadStmtNumResultTable = { { pql1, pqlR1 } };
    Table xEqualsXResultTable = { {PQLEntity::generateConstant(0)} };

    ClauseResult selectStmtResult = ClauseResult({ stmtArg }, selectStmtResultTable);
    ClauseResult selectAssignResult = ClauseResult({ assignArg }, selectAssignResultTable);
    ClauseResult selectReadResult = ClauseResult({ readArg }, selectReadResultTable);
    ClauseResult selectWhileResult = ClauseResult({ whileArg }, selectWhileResultTable);
    ClauseResult selectIfResult = ClauseResult({ ifArg }, selectIfResultTable);
    ClauseResult selectCallResult = ClauseResult({ callArg }, selectCallResultTable);
    ClauseResult selectPrintResult = ClauseResult({ printArg }, selectPrintResultTable);
    ClauseResult selectProcResult = ClauseResult({ procArg }, selectProcResultTable);
    ClauseResult selectVarResult = ClauseResult({ varArg }, selectVarResultTable);
    ClauseResult selectConstResult = ClauseResult({ constArg }, selectConstResultTable);

    ClauseResult followsReadAssignResult = ClauseResult({ readArg, assignArg },
        { pqlFollowsR1A2 });
    ClauseResult modifiesAssignVarResult = ClauseResult({ assignArg, varArg },
        { pqlModifiesSA2X, pqlModifiesSA6X, pqlModifiesSA8Y, pqlModifiesSA9Y });
    ClauseResult modifiesAssignZResult = ClauseResult({ assignArg, ZStringLiteralArg }, Table{});
    ClauseResult parentStarWhileAssign2Result = ClauseResult({ whileArg, assign2Arg },
        { pqlParentStarW3A6 });
    ClauseResult parentIfCallResult = ClauseResult({ ifArg, callArg },
        { pqlParentI7C10 });
    ClauseResult callsProcProc2Result = ClauseResult({ procArg, proc2Arg }, { pqlCallsPXPMain, pqlCallsPYPX });

    ClauseResult patternAssign2XResult = ClauseResult({ assign2Arg, XStringLiteralArg },
        { pqlPatternA2X, pqlPatternA6X });

    ClauseResult withCallProcNameVarVarNameResult = ClauseResult(
        { callArg, callProcNameAttributeArg, varArg, variableVarNameAttributeArg },
        callProcNameVarVarNameResultTable);
    ClauseResult withConstValueReadStmtNumResult = ClauseResult(
        { constArg, constValueAttributeArg, readArg, readStmtNumAttributeArg },
        constValueReadStmtNumResultTable);
    ClauseResult withXEqualsXResult = ClauseResult({ wildcardArg }, xEqualsXResultTable);
    ClauseResult withOneEqualsTwoResult = ClauseResult({ lineNumber1Arg, lineNumber2Arg },
        Table{});

    ClauseResult selectBooleanPlaceholderResult = ClauseResult(
        { ClauseArgument::createWildcardArg() },
        Table{ {PQLEntity::generateConstant(0)} });

    /* Returns true if ClauseResults are grouped correctly, regardless of order within the group */
    bool hasSameGroups(vector<vector<ClauseResult>> actualGroups,
        vector<vector<ClauseResult>> expectedGroups) {
        for (vector<ClauseResult>& group : actualGroups) {
            sort(group.begin(), group.end());
        }
        for (vector<ClauseResult>& group : expectedGroups) {
            sort(group.begin(), group.end());
        }
        sort(actualGroups.begin(), actualGroups.end());
        sort(expectedGroups.begin(), expectedGroups.end());

        return actualGroups == expectedGroups;
    }

    /* Returns true if the ordering of ClauseResults within the group is valid.
     * i.e. every ClauseResult x other than the first has at least 1 preceding
     *      ClauseResult y with at least one argument same as x's arguments
     */
    bool isValidOrder(vector<ClauseResult> actualGroup) {
        if (actualGroup.size() <= 2) {
            return true;
        }

        unordered_set<ClauseArgument> previousArgs;
        for (ClauseArgument arg : actualGroup.front().getSynonymArgs()) {
            previousArgs.insert(arg);
        }
        for (ClauseResult& result : actualGroup) {
            if (result == actualGroup.front()) {
                continue;
            }
            // Check if current result is in a valid position
            bool hasAtLeastOneArgInPrecedingClauses = false;
            for (const ClauseArgument& arg : result.getSynonymArgs()) {
                hasAtLeastOneArgInPrecedingClauses = previousArgs.count(arg) > 0
                    || hasAtLeastOneArgInPrecedingClauses;
            }
            // Insertion done in separate loop to account for result with same lhs and rhs args
            for (const ClauseArgument& arg : result.getSynonymArgs()) {
                previousArgs.insert(arg);
            }
            if (!hasAtLeastOneArgInPrecedingClauses) {
                return false;
            }
        }
        // All results are in a valid position
        return true;
    }
}

// ==================== UNIT TESTS ====================

TEST_CASE("QueryResultsOptimiser: test optimise (empty or no constraint queries)") {
    auto testOptimise = [](list<ClauseResult> selectResults,
            list<ClauseResult> relationshipResults, list<ClauseResult> withResults,
            vector<vector<vector<ClauseResult>>> expectedGroups, bool expectedIsEmptyResultFound) {
                // given
                QueryResultsOptimiser optimiser = QueryResultsOptimiser(selectResults,
                    relationshipResults, withResults);
                bool actualIsEmptyResultFound = false;

                // when
                vector<vector<vector<ClauseResult>>> actualGroups = optimiser.optimise(actualIsEmptyResultFound);

                // then
                REQUIRE(actualGroups == expectedGroups);
                REQUIRE(actualIsEmptyResultFound == expectedIsEmptyResultFound);
    };

    // TODO: Write tests for optimise
    list<ClauseResult> selectResults;
    list<ClauseResult> relationshipResults;
    list<ClauseResult> withResults;
    vector<vector<vector<ClauseResult>>> expectedGroups;
    bool isEmpty;

    SECTION("Empty result") {
        isEmpty = true;

        //relationship has 1 empty result
        selectResults = { selectStmtResult, selectProcResult, selectAssignResult, };
        relationshipResults = { followsReadAssignResult, modifiesAssignZResult, };
        withResults = { withCallProcNameVarVarNameResult, withXEqualsXResult };
        expectedGroups = { {}, {} };
        testOptimise(selectResults, relationshipResults, withResults, expectedGroups, isEmpty);

        //with has 1 empty result
        selectResults = { selectStmtResult, selectProcResult, selectAssignResult, };
        relationshipResults = { followsReadAssignResult, parentStarWhileAssign2Result, };
        withResults = { withCallProcNameVarVarNameResult, withOneEqualsTwoResult };
        expectedGroups = { {}, {} };
        testOptimise(selectResults, relationshipResults, withResults, expectedGroups, isEmpty);

    }

    SECTION("No constraints") {
        isEmpty = false;

        //relationship has 1 empty result
        selectResults = { selectConstResult };
        relationshipResults = {};
        withResults = {};
        expectedGroups = { {}, {} };
        testOptimise(selectResults, relationshipResults, withResults, expectedGroups, isEmpty);

    }

}

TEST_CASE("QueryResultsOptimiser: test optimise (non-empty queries)") {

    auto testOptimise = [](list<ClauseResult> selectResults,
            list<ClauseResult> relationshipResults, list<ClauseResult> withResults,
            const vector<vector<vector<ClauseResult>>> expectedUnorderedGroups) {
                // given
                QueryResultsOptimiser optimiser = QueryResultsOptimiser(selectResults,
                    relationshipResults, withResults);
                bool actualIsEmptyResultFound = false;

                // when
                vector<vector<vector<ClauseResult>>> actualGroups = 
                    optimiser.optimise(actualIsEmptyResultFound);

                // then
                REQUIRE(hasSameGroups(actualGroups.front(), expectedUnorderedGroups.front()));
                REQUIRE(hasSameGroups(actualGroups.back(), expectedUnorderedGroups.back()));
                for (vector<ClauseResult> group : actualGroups.front()) {
                    REQUIRE(isValidOrder(group));
                }
                for (vector<ClauseResult> group : actualGroups.back()) {
                    REQUIRE(isValidOrder(group));
                }
    };

    // TODO: Write tests for optimise
    list<ClauseResult> selectResults;
    list<ClauseResult> relationshipResults;
    list<ClauseResult> withResults;
    vector<vector<vector<ClauseResult>>> expectedUnorderedGroups;


    SECTION("Select and Non-Select groups present, at least one group forms a tree") {
        selectResults = { selectConstResult, selectCallResult };
        relationshipResults = {
            modifiesAssignVarResult,parentIfCallResult, followsReadAssignResult,
            parentStarWhileAssign2Result, patternAssign2XResult, callsProcProc2Result,
        };
        withResults = { withCallProcNameVarVarNameResult, withConstValueReadStmtNumResult, };
        expectedUnorderedGroups = {
            {
                {modifiesAssignVarResult, parentIfCallResult, followsReadAssignResult,
                    withConstValueReadStmtNumResult, withCallProcNameVarVarNameResult},
            },
            {
                {parentStarWhileAssign2Result, patternAssign2XResult},
                {callsProcProc2Result},
            }
        };
        testOptimise(selectResults, relationshipResults, withResults, expectedUnorderedGroups);
    }

}