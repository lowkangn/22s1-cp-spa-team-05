#include "catch.hpp"
#include "../../../spa/src/qps/query_evaluator/QueryEvaluator.h"
#include "../../../spa/src/qps/query_evaluator/QueryEvaluator.cpp"

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("QueryEvaluator: test combine") {
auto testCombine = [](pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>> results,
                                         string expected) {
    // given
    QueryEvaluator evaluator = QueryEvaluator();

    // when
    string actual = evaluator.combine(results);

    // then
    REQUIRE(actual == expected);
    };


    SECTION("SelectClause only") {
        // x = x + 1; variable v; Select v
        testCombine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>>{
                            shared_ptr<ClauseResult>(new EntityClauseResult(
                                            ClauseArgument("v", ArgumentType::ENTREF_SYNONYM),
                                            vector<PQLEntity>{
                                                PQLEntity::generateVariable("x")
                                            })),
                            list<shared_ptr<ClauseResult>>{}
                    },
                    "x");

        // x = x + 1; constant c; Select c
        testCombine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>>{
                            shared_ptr<ClauseResult>(new EntityClauseResult(
                                    ClauseArgument("c", ArgumentType::ENTREF_SYNONYM),
                                    vector<PQLEntity>{
                                            PQLEntity::generateConstant(1)
                                    })),
                            list<shared_ptr<ClauseResult>>{}
                    },
                    "1");

        // x = x + 1; procedure p; Select p
        testCombine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>>{
                            shared_ptr<ClauseResult>(new EntityClauseResult(
                                    ClauseArgument("p", ArgumentType::ENTREF_SYNONYM),
                                    vector<PQLEntity>{})),
                            list<shared_ptr<ClauseResult>>{}
                    },
                    "");
    }

    SECTION("SelectClause with ModifiesSClause") {

        PQLEntity relationship1Entity1 = PQLEntity::generateStatement(1);
        PQLEntity relationship1Entity2 = PQLEntity::generateVariable("x");
        PQLEntity relationship2Entity1 = PQLEntity::generateStatement(2);
        PQLEntity relationship2Entity2 = PQLEntity::generateVariable("y");

        // x = x + 1; y = y + 1; assign a; variable v; Select a such that Modifies(a,v)
        testCombine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>>{
                            shared_ptr<ClauseResult>(new EntityClauseResult(
                                    ClauseArgument("a", ArgumentType::STMTREF_SYNONYM),
                                    vector<PQLEntity>{
                                            PQLEntity::generateStatement(1),
                                            PQLEntity::generateStatement(2)
                                    })),
                            list<shared_ptr<ClauseResult>>{
                                shared_ptr<ClauseResult>(new RelationshipClauseResult(
                                        ClauseArgument("a", ArgumentType::STMTREF_SYNONYM),
                                        ClauseArgument("v", ArgumentType::ENTREF_SYNONYM),
                                        vector<PQLRelationship>{
                                                PQLRelationship(relationship1Entity1, relationship1Entity2),
                                                PQLRelationship(relationship2Entity1, relationship2Entity2)
                                        }))
                            }
                    },
                    "2, 1");

        // x = x + 1; y = y + 1; assign a; variable v; Select v such that Modifies(a,v)
        testCombine(pair<shared_ptr<ClauseResult>, list<shared_ptr<ClauseResult>>>{
                            shared_ptr<ClauseResult>(new EntityClauseResult(
                                    ClauseArgument("v", ArgumentType::ENTREF_SYNONYM),
                                    vector<PQLEntity>{
                                            PQLEntity::generateVariable("x"),
                                            PQLEntity::generateVariable("y")
                                    })),
                            list<shared_ptr<ClauseResult>>{
                                    shared_ptr<ClauseResult>(new RelationshipClauseResult(
                                            ClauseArgument("a", ArgumentType::STMTREF_SYNONYM),
                                            ClauseArgument("v", ArgumentType::ENTREF_SYNONYM),
                                            vector<PQLRelationship>{
                                                    PQLRelationship(relationship1Entity1, relationship1Entity2),
                                                    PQLRelationship(relationship2Entity1, relationship2Entity2)
                                            }))
                            }
                    },
                    "y, x");
    }


}
