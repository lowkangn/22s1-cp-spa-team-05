#include "catch.hpp"
#include "../../../spa/src/qps/query/clause/RelationshipClauseResult.h"

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("RelationshipClauseResult: test operator==") {
    auto testOperatorEquals = [](RelationshipClauseResult first, RelationshipClauseResult second, bool expected) {
        // when
        bool actual = first == second;

        // then
        REQUIRE(actual == expected);
    };

    ClauseArgument firstArg = ClauseArgument("v", ArgumentType::VARIABLE);
    ClauseArgument firstArgAgain = ClauseArgument("v", ArgumentType::VARIABLE);
    ClauseArgument secondArg = ClauseArgument("a", ArgumentType::ASSIGN);
    ClauseArgument thirdArg = ClauseArgument("1", ArgumentType::LINE_NUMBER);

    PQLEntity firstEntity = PQLEntity::generateVariable("x");
    PQLEntity secondEntity = PQLEntity::generateVariable("y");
    PQLEntity thirdEntity = PQLEntity::generateVariable("z");

    PQLRelationship firstRelationship = PQLRelationship(firstEntity, secondEntity);
    PQLRelationship firstRelationshipAgain = PQLRelationship(firstEntity, secondEntity);
    PQLRelationship secondRelationship = PQLRelationship(firstEntity, thirdEntity);
    PQLRelationship thirdRelationship = PQLRelationship(secondEntity, thirdEntity);

    SECTION("Equal") {
        // Same args and relationships
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           true);

        // Different args but same parameters, same relationships
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArgAgain, secondArg, vector<PQLRelationship>{firstRelationship}),
                           true);

        // Same args, different sets of relationships but same parameters
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationshipAgain}),
                           true);

        // Different arg but same parameters, different sets of relationships but same parameters
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArgAgain, secondArg, vector<PQLRelationship>{firstRelationshipAgain}),
                           true);
    }

    SECTION("Not equal") {
        // Different args, same relationships
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(secondArg, thirdArg, vector<PQLRelationship>{firstRelationship}),
                           false);

        // Same args but in different order, same relationships
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(secondArg, firstArg, vector<PQLRelationship>{firstRelationship}),
                           false);

        // Same args, different relationships
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{secondRelationship}),
                           false);

        // Same args, same sets of relationships but in different order
        testOperatorEquals(RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{firstRelationship, secondRelationship}),
                           RelationshipClauseResult(firstArg, secondArg, vector<PQLRelationship>{secondRelationship, firstRelationship}),
                           false);
    }
}