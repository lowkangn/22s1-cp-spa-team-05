#include "catch.hpp"
#include <list>
#include <qps/query_parser/ArgumentType.h>
using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ArgumentType: test getDesignEntityArgumentType") {
    auto testGetDesignEntityArgumentType= [](string argumentType, bool shouldThrow) {
        if (shouldThrow) {
            REQUIRE_THROWS_AS(getDesignEntityArgumentType(argumentType),
                PQLSyntaxError);
        } else {
            REQUIRE_NOTHROW(getDesignEntityArgumentType(argumentType));
        }
    };

    SECTION("Not a design entity") {
        testGetDesignEntityArgumentType("nonExistentDesignEntity", true);
    }

    SECTION("Design entities with typo") {
        testGetDesignEntityArgumentType("Variable", true);
        testGetDesignEntityArgumentType("procdure", true);
        testGetDesignEntityArgumentType("statement", true);
    }

    SECTION("Existing design entities") {
        testGetDesignEntityArgumentType("stmt", false);
        testGetDesignEntityArgumentType("read", false);
        testGetDesignEntityArgumentType("print", false);
        testGetDesignEntityArgumentType("assign", false);
        testGetDesignEntityArgumentType("call", false);
        testGetDesignEntityArgumentType("while", false);
        testGetDesignEntityArgumentType("if", false);
        testGetDesignEntityArgumentType("procedure", false);
        testGetDesignEntityArgumentType("variable", false);
        testGetDesignEntityArgumentType("constant", false);
    }
}