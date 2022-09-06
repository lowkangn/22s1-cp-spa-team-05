#include "catch.hpp"
#include "../../../spa/src/qps/query/clause/EntityClauseResult.h"
#include "../../../spa/src/qps/query/clause/EntityClauseResult.cpp"

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("EntityClauseResult: test operator==") {
    auto testOperatorEquals = [](EntityClauseResult first, EntityClauseResult second, bool expected) {
        // when
        bool actual = first == second;

        // then
        REQUIRE(actual == expected);
    };

    ClauseArgument firstArg = ClauseArgument("v", ArgumentType::ENTREF_SYNONYM);
    ClauseArgument firstArgAgain = ClauseArgument("v", ArgumentType::ENTREF_SYNONYM);
    ClauseArgument secondArg = ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM);

    PQLEntity firstEntity = PQLEntity::generateVariable("x");
    PQLEntity firstEntityAgain = PQLEntity::generateVariable("x");
    PQLEntity secondEntity = PQLEntity::generateVariable("y");
    PQLEntity thirdEntity = PQLEntity::generateVariable("z");

    SECTION("Equal") {
        // Same arg and entities
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           true);

        // Different arg with same parameters, same entities
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArgAgain, vector<PQLEntity>{firstEntity}),
                           true);

        // Same arg, different entities with same parameters
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{firstEntityAgain}),
                           true);

        // Different arg with same parameters, different entities with same parameters
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArgAgain, vector<PQLEntity>{firstEntityAgain}),
                           true);

        // Same arg and multiple sets of entities
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity, secondEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity, secondEntity}),
                           true);
    }

    SECTION("Not equal") {

        // Different args, same entities
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(secondArg, vector<PQLEntity>{firstEntity}),
                           false);

        // Same args, different entities
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{secondEntity}),
                           false);

        // Same args, different sets of entities
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity, secondEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity, thirdEntity}),
                           false);

        // Same args, same sets of entities but in different order
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity, secondEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{secondEntity, firstEntity}),
                           false);
    }
}