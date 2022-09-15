#include "catch.hpp"
#include "../../../spa/src/qps/query/clause/PQLRelationship.h"

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("PQLRelationship: test operator==") {
    auto testOperatorEquals = [](PQLRelationship first, PQLRelationship second, bool expected) {
        // when
        bool actual = first == second;

        // then
        REQUIRE(actual == expected);
    };

    PQLEntity firstVarEntity = PQLEntity::generateVariable("first");
    PQLEntity secondVarEntity = PQLEntity::generateVariable("second");
    PQLEntity thirdVarEntity = PQLEntity::generateVariable("third");

    PQLRelationship firstRelationship = PQLRelationship(firstVarEntity, secondVarEntity);
    PQLRelationship firstRelationshipAgain = PQLRelationship(firstVarEntity, secondVarEntity);
    PQLRelationship firstRelationshipFlipped = PQLRelationship(secondVarEntity, firstVarEntity);
    PQLRelationship secondRelationship = PQLRelationship(firstVarEntity, thirdVarEntity);

    SECTION("Equal") {
        testOperatorEquals(firstRelationship, firstRelationship, true);
        testOperatorEquals(firstRelationship, firstRelationshipAgain, true);
    }

    SECTION("Not equal") {
        testOperatorEquals(firstRelationship, secondRelationship, false);
        testOperatorEquals(firstRelationshipAgain, secondRelationship, false);
        testOperatorEquals(firstRelationship, firstRelationshipFlipped, false);
        testOperatorEquals(firstRelationshipAgain, firstRelationshipFlipped, false);
    }

}