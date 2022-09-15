#include "catch.hpp"
#include "../../../spa/src/qps/query/clause/PQLEntity.h"

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("PQLEntity: test operator==") {
    auto testOperatorEquals = [](PQLEntity first, PQLEntity second, bool expected) {
        // when
        bool actual = first == second;

        // then
        REQUIRE(actual == expected);
    };

    PQLEntity firstVarEntity = PQLEntity::generateVariable("first");
    PQLEntity firstVarEntityAgain = PQLEntity::generateVariable("first");
    PQLEntity secondVarEntity = PQLEntity::generateVariable("second");

    PQLEntity firstProcEntity = PQLEntity::generateProcedure("first");
    PQLEntity firstProcEntityAgain = PQLEntity::generateProcedure("first");
    PQLEntity secondProcEntity = PQLEntity::generateProcedure("second");

    PQLEntity firstConstEntity = PQLEntity::generateConstant(1);
    PQLEntity firstConstEntityAgain = PQLEntity::generateConstant(1);
    PQLEntity secondConstEntity = PQLEntity::generateConstant(2);

    PQLEntity firstStmtEntity = PQLEntity::generateStatement(1);
    PQLEntity firstStmtEntityAgain = PQLEntity::generateStatement(1);
    PQLEntity secondStmtEntity = PQLEntity::generateStatement(2);

    SECTION("Equal") {
        testOperatorEquals(firstVarEntity, firstVarEntity, true);
        testOperatorEquals(firstVarEntity, firstVarEntityAgain, true);

        testOperatorEquals(firstProcEntity, firstProcEntity, true);
        testOperatorEquals(firstProcEntity, firstProcEntityAgain, true);

        testOperatorEquals(firstConstEntity, firstConstEntity, true);
        testOperatorEquals(firstConstEntity, firstConstEntityAgain, true);

        testOperatorEquals(firstStmtEntity, firstStmtEntity, true);
        testOperatorEquals(firstStmtEntity, firstStmtEntityAgain, true);
    }

    SECTION("Not equal") {
        testOperatorEquals(firstVarEntity, secondVarEntity, false);
        testOperatorEquals(firstVarEntityAgain, secondVarEntity, false);

        testOperatorEquals(firstProcEntity, secondVarEntity, false);
        testOperatorEquals(firstProcEntityAgain, secondVarEntity, false);

        testOperatorEquals(firstConstEntity, secondVarEntity, false);
        testOperatorEquals(firstConstEntityAgain, secondVarEntity, false);

        testOperatorEquals(firstStmtEntity, secondVarEntity, false);
        testOperatorEquals(firstStmtEntityAgain, secondVarEntity, false);
    }

}

TEST_CASE("PQLEntity: test toString") {
    auto testToStringEquals = [](PQLEntity entity, string expected) {
        // when
        string actual = entity.toString();

        // then
        REQUIRE(actual == expected);
    };

    PQLEntity varEntity = PQLEntity::generateVariable("first");
    PQLEntity procEntity = PQLEntity::generateProcedure("second");
    PQLEntity constEntity = PQLEntity::generateConstant(1);
    PQLEntity stmtEntity = PQLEntity::generateStatement(2);

    SECTION("Equal") {
        testToStringEquals(varEntity, "first");
        testToStringEquals(procEntity, "second");
        testToStringEquals(constEntity, "1");
        testToStringEquals(stmtEntity, "2");
    }
}
