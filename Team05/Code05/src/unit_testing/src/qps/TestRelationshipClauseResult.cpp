#include "catch.hpp"
#include "../../../spa/src/qps/query/clause/RelationshipClauseResult.h"
#include "../../../spa/src/qps/query/clause/RelationshipClauseResult.cpp"

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("RelationshipClauseResult: test operator==") {
    auto testOperatorEquals = [](RelationshipClauseResult first, RelationshipClauseResult second, bool expected) {
        // when
        bool actual = first == second;

        // then
        REQUIRE(actual == expected);
    };

    ClauseArgument firstArg = ClauseArgument("v", ArgumentType::ENTREF_SYNONYM);
    ClauseArgument firstArgAgain = ClauseArgument("v", ArgumentType::ENTREF_SYNONYM);
    ClauseArgument secondArg = ClauseArgument("a", ArgumentType::STMTREF_SYNONYM);
    ClauseArgument thirdArg = ClauseArgument("1", ArgumentType::LINE_NUMBER);

    PQLEntity firstEntity = PQLEntity::generateVariable("x");
    PQLEntity firstEntityAgain = PQLEntity::generateVariable("x");
    PQLEntity secondEntity = PQLEntity::generateVariable("y");
    PQLEntity thirdEntity = PQLEntity::generateVariable("z");

    PQLRelationship firstRelationship = PQLRelationship(firstEntity, secondEntity);
    PQLRelationship firstRelationshipAgain = PQLRelationship(firstEntityAgain, secondEntity);
    PQLRelationship secondRelationship = PQLRelationship(firstEntityAgain, thirdEntity);
    PQLRelationship thirdRelationship = PQLRelationship(secondEntity, thirdEntity);

    SECTION("Equal") {
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           true);
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArgAgain, secondArg, vector<PQLRelationship>{firstRelationship}),
                           true);
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationshipAgain}),
                           true);
    }

    SECTION("Not equal") {
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(secondArg, firstArg, vector<PQLRelationship>{firstRelationship}),
                           false);
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{secondRelationship}),
                           false);
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship, secondRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{secondRelationship, firstRelationship}),
                           false);
    }
}