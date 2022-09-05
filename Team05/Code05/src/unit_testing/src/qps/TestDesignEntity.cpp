#include "catch.hpp"
#include <string>
#include <list>
#include <qps/query_parser/PQLError.h>
#include <qps/query_parser/DesignEntity.h>
#include <qps/query_parser/DesignEntity.cpp>
using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("Declaration: test getDesignEntity") {
    auto testGetDesignEntity= [](string designEntity, bool shouldThrow) {
        if (shouldThrow) {
            REQUIRE_THROWS_AS(getDesignEntity(designEntity),
                PQLError);
        } else {
            REQUIRE_NOTHROW(getDesignEntity(designEntity));
        }
    };

    SECTION("Not a design entity") {
        testGetDesignEntity("nonExistantDesignEntity", true);
    }

    SECTION("Design entities with typo") {
        testGetDesignEntity("Variable", true);
        testGetDesignEntity("procdure", true);
        testGetDesignEntity("statement", true);
    }

    SECTION("Existing design entities") {
        testGetDesignEntity("call", false);
        testGetDesignEntity("variable", false);
        testGetDesignEntity("stmt", false);
    }
}