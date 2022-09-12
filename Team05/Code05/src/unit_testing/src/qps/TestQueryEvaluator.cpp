#include "catch.hpp"
#include "../../../spa/src/qps/query_evaluator/QueryEvaluator.h"
#include "../../../spa/src/qps/query_evaluator/QueryEvaluator.cpp"

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("QueryEvaluator: test combine") {
auto testCombine = [](pair<shared_ptr<EntityClauseResult>,
		list<shared_ptr<RelationshipClauseResult>>> results, unordered_set<string> expected) {
    // given
    QueryEvaluator evaluator = QueryEvaluator();

    // when
    unordered_set<string> actual = evaluator.combine(results);

    // then
    REQUIRE(actual == expected);
    };


    SECTION("SelectClause only") {
        // x = x + 1; variable v; Select v
        ClauseArgument variableArg = ClauseArgument("v", ArgumentType::VARIABLE);
        PQLEntity varEntity = PQLEntity::generateVariable("x");
        shared_ptr<EntityClauseResult> entityClauseResultWithVariable =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						variableArg,vector<PQLEntity>{varEntity}));

        pair<shared_ptr<EntityClauseResult>, list<shared_ptr<RelationshipClauseResult>>> combineInput = {
                entityClauseResultWithVariable,
                list<shared_ptr<RelationshipClauseResult>>{}
        };

        unordered_set<string> expectedSet = unordered_set<string>{"x"};

        testCombine(combineInput, expectedSet);

        // x = x + 1; constant c; Select c
        ClauseArgument constantArg = ClauseArgument("c", ArgumentType::CONSTANT);
        PQLEntity constEntity = PQLEntity::generateConstant(1);
        shared_ptr<EntityClauseResult> entityClauseResultWithConstant =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						constantArg,vector<PQLEntity>{constEntity}));

        combineInput = {
                entityClauseResultWithConstant,
                list<shared_ptr<RelationshipClauseResult>>{}
        };

        expectedSet = unordered_set<string>{"1"};

        testCombine(combineInput, expectedSet);

        // x = x + 1; procedure p; Select p
        ClauseArgument procArg = ClauseArgument("p", ArgumentType::PROCEDURE);
        shared_ptr<EntityClauseResult> entityClauseResultWithProcedure =
				shared_ptr<EntityClauseResult>(new EntityClauseResult(
						procArg,vector<PQLEntity>{}));

        combineInput = {
                entityClauseResultWithProcedure,
                list<shared_ptr<RelationshipClauseResult>>{}
        };

        expectedSet = unordered_set<string>{};

        testCombine(combineInput, expectedSet);
    }

    SECTION("SelectClause with ModifiesSClause") {

        ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
        ClauseArgument varArg = ClauseArgument("v", ArgumentType::VARIABLE);

        PQLEntity relationship1Entity1 = PQLEntity::generateStatement(1);
        PQLEntity relationship1Entity2 = PQLEntity::generateVariable("x");
        PQLEntity relationship2Entity1 = PQLEntity::generateStatement(2);
        PQLEntity relationship2Entity2 = PQLEntity::generateVariable("y");

        PQLRelationship relationship1 = PQLRelationship(relationship1Entity1, relationship1Entity2);
        PQLRelationship relationship2 = PQLRelationship(relationship2Entity1, relationship2Entity2);

        vector<PQLEntity> stmtEntities = vector<PQLEntity>{relationship1Entity1, relationship2Entity1};
        vector<PQLEntity> varEntities = vector<PQLEntity>{relationship1Entity2, relationship2Entity2};

        shared_ptr<EntityClauseResult> assignEntityClauseResult = shared_ptr<EntityClauseResult>(
				new EntityClauseResult(assignArg, stmtEntities));

        shared_ptr<RelationshipClauseResult> assignAndVarRelationshipClauseResult(new RelationshipClauseResult(
                assignArg,
                varArg,
                vector<PQLRelationship>{relationship1, relationship2}));

        pair<shared_ptr<EntityClauseResult>, list<shared_ptr<RelationshipClauseResult>>> combineInput = {
                assignEntityClauseResult,
                list<shared_ptr<RelationshipClauseResult>>{assignAndVarRelationshipClauseResult}
        };

        unordered_set<string> expectedSet = unordered_set<string>{"1", "2"};

        // x = x + 1; y = y + 1; assign a; variable v; Select a such that Modifies(a,v)
        testCombine(combineInput, expectedSet);

        // x = x + 1; y = y + 1; assign a; variable v; Select v such that Modifies(a,v)
        shared_ptr<EntityClauseResult> varEntityClauseResult = shared_ptr<EntityClauseResult>(
				new EntityClauseResult(varArg, varEntities));

        combineInput = {
                varEntityClauseResult,
                list<shared_ptr<RelationshipClauseResult>>{assignAndVarRelationshipClauseResult}
        };
        expectedSet = unordered_set<string>{"x", "y"};

        testCombine(combineInput, expectedSet);
    }

}
