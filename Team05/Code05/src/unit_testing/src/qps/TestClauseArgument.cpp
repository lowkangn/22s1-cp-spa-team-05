#include "catch.hpp"
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseArgument.cpp>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ClauseArgument: test operator==") {
    auto testOperatorEquals = [](ClauseArgument first, ClauseArgument second, bool expected) {
            // when
            bool actual = first == second;
            
            // then
            REQUIRE(actual == expected);
    };


    SECTION("Equal") {
        testOperatorEquals(ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM),
            ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM), true);
        testOperatorEquals(ClauseArgument("simpleName", ArgumentType::STRING_LITERAL),
            ClauseArgument("simpleName", ArgumentType::STRING_LITERAL), true);
        testOperatorEquals(ClauseArgument("1", ArgumentType::LINE_NUMBER),
            ClauseArgument("1", ArgumentType::LINE_NUMBER), true);
        testOperatorEquals(ClauseArgument("_", ArgumentType::WILDCARD),
            ClauseArgument("_", ArgumentType::WILDCARD), true);
    }

    SECTION("Not equal") {
        testOperatorEquals(ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM),
            ClauseArgument("v2", ArgumentType::ENTREF_SYNONYM), false);
        testOperatorEquals(ClauseArgument("simpleName", ArgumentType::ENTREF_SYNONYM),
            ClauseArgument("simpleName", ArgumentType::STRING_LITERAL), false);
        testOperatorEquals(ClauseArgument("a1", ArgumentType::STMTREF_SYNONYM),
            ClauseArgument("1", ArgumentType::LINE_NUMBER), false);
        testOperatorEquals(ClauseArgument("_", ArgumentType::WILDCARD),
            ClauseArgument("w1", ArgumentType::STMTREF_SYNONYM), false);
    }

}

