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

    SECTION("Equal") {
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           true);

        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArgAgain, vector<PQLEntity>{firstEntity}),
                           true);

        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{firstEntityAgain}),
                           true);

        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArgAgain, vector<PQLEntity>{firstEntityAgain}),
                           true);
    }

    SECTION("Not equal") {
        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(secondArg, vector<PQLEntity>{firstEntity}),
                           false);

        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{secondEntity}),
                           false);

        testOperatorEquals(EntityClauseResult(firstArg, vector<PQLEntity>{firstEntity, secondEntity}),
                           EntityClauseResult(firstArg, vector<PQLEntity>{secondEntity, firstEntity}),
                           false);
    }
}