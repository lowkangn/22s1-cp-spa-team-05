#include "catch.hpp"
#include <list>
#include <qps/exceptions/PQLError.h>
#include <qps/query_parser/ArgumentType.h>
using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ArgumentType: test getArgumentType") {
    auto testGetDesignEntity= [](string argumentType, bool shouldThrow) {
        if (shouldThrow) {
            REQUIRE_THROWS_AS(getDesignEntityArgumentType(argumentType),
                PQLError);
        } else {
            REQUIRE_NOTHROW(getDesignEntityArgumentType(argumentType));
        }
    };

    SECTION("Not a design entity") {
        testGetDesignEntity("nonExistentDesignEntity", true);
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