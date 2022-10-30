#include "catch.hpp"
#include <qps/results_projector/QueryResultsProjector.h>

TEST_CASE("QueryResultsProjector: test populateResultsList") {
    auto testListPopulatedCorrectly = [](set<string>expectedSet, ClauseResult evaluatorResult, bool isReturningBoolean,
        string errorString) {
        // given
        list<string> autotesterList;

        // when
        QueryResultsProjector projector = QueryResultsProjector(evaluatorResult);
        projector.populateResultsList(autotesterList, isReturningBoolean, errorString);

        // then
        set<string> autotesterSet {};
        for (string s : autotesterList) {
            autotesterSet.insert(s);
        }
        REQUIRE(autotesterSet == expectedSet);
    };

    PQLEntity pqlR1 = PQLEntity::generateStatement(1);
    PQLEntity pqlA2 = PQLEntity::generateStatement(2);
    PQLEntity pqlW3 = PQLEntity::generateStatement(3);

    PQLEntity pqlMain = PQLEntity::generateProcedure("main");
    PQLEntity pqlProcX = PQLEntity::generateProcedure("x");
    
    PQLEntity pqlY = PQLEntity::generateVariable("y");
    PQLEntity pqlZ = PQLEntity::generateVariable("z");

    ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
    ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
    ClauseArgument varArg = ClauseArgument::createVariableArg("v");

    ClauseResult evaluatorResult;
    Table entitiesInResults;

    SECTION("Boolean results") {
        bool isBoolean = true;
        string errorString = "";

        entitiesInResults = { {pqlR1}, {pqlA2}, {pqlW3} };
        evaluatorResult = ClauseResult({ stmtArg }, entitiesInResults);
        testListPopulatedCorrectly(set<string> {"TRUE"}, evaluatorResult,
            isBoolean, errorString);

        evaluatorResult = EntityClauseResult::createNonEmptyNoSynonymResult();
        testListPopulatedCorrectly(set<string> {"TRUE"}, evaluatorResult,
            isBoolean, errorString);

        entitiesInResults = { };
        evaluatorResult = ClauseResult({ procArg }, entitiesInResults);
        testListPopulatedCorrectly(set<string> {"FALSE"}, evaluatorResult,
            isBoolean, errorString);

        evaluatorResult = EntityClauseResult::createEmptyNoSynonymResult();
        testListPopulatedCorrectly(set<string> {"FALSE"}, evaluatorResult,
            isBoolean, errorString);
    };
    
    SECTION("Empty synonym results") {
        bool isBoolean = false;
        string errorString = "";

        entitiesInResults = { };
        evaluatorResult = ClauseResult({ stmtArg }, entitiesInResults);
        testListPopulatedCorrectly(set<string> {},evaluatorResult, isBoolean, errorString);
        
        evaluatorResult = ClauseResult({ stmtArg, procArg, varArg }, 
            entitiesInResults);
        testListPopulatedCorrectly(set<string> {}, evaluatorResult, isBoolean, errorString);
    }

    SECTION("Nonempty single synonym result") {
        bool isBoolean = false;
        string errorString = "";

        entitiesInResults = { {pqlR1}, {pqlA2}, {pqlW3} };
        evaluatorResult = ClauseResult({ stmtArg }, entitiesInResults);
        testListPopulatedCorrectly(set<string> {"1", "2", "3"}, evaluatorResult, 
            isBoolean, errorString);

        entitiesInResults = { {pqlMain}, {pqlProcX} };
        evaluatorResult = ClauseResult({ procArg }, entitiesInResults);
        testListPopulatedCorrectly(set<string> {"main", "x"}, evaluatorResult, 
            isBoolean, errorString);

        entitiesInResults = { {pqlY}, {pqlZ} };
        evaluatorResult = ClauseResult({ varArg }, entitiesInResults);
        testListPopulatedCorrectly(set<string> {"y", "z"}, evaluatorResult,
            isBoolean, errorString);
    };

    SECTION("Nonempty multi synonym result") {
        bool isBoolean = false;
        string errorString = "";

        entitiesInResults = { {pqlR1, pqlMain}, {pqlR1, pqlProcX}, {pqlA2, pqlMain},
            {pqlA2, pqlProcX}, {pqlW3, pqlMain}, {pqlW3, pqlProcX}, };
        evaluatorResult = ClauseResult({ stmtArg, procArg }, entitiesInResults);
        testListPopulatedCorrectly(
            set<string> {"1 main", "1 x", "2 main", "2 x", "3 main", "3 x", },
            evaluatorResult, isBoolean, errorString);
    };

    SECTION("Errors") {
        for (string errorString : {"SyntaxError", "SemanticError"}) {
            testListPopulatedCorrectly(set<string> {errorString}, evaluatorResult, true, errorString);
            testListPopulatedCorrectly(set<string> {errorString}, evaluatorResult, false, errorString);
        }
    };

};
