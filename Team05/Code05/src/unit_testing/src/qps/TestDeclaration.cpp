#include "catch.hpp"
#include <string>
#include <list>
#include <qps/query_parser/PQLError.h>
#include <qps/query_parser/Declaration.h>
#include <qps/query_parser/Declaration.cpp>
using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("Declaration: test Constructor") {
    auto testConstructor= [](string designEntity, string synonym, bool shouldThrow) {
        if (shouldThrow) {
            REQUIRE_THROWS_AS(new Declaration(designEntity, synonym),
                PQLError);
        } else {
            REQUIRE_NOTHROW(new Declaration(designEntity, synonym));
        }
    };

    string synonym = "a";

    SECTION("Not a design entity") {
        testConstructor("nonExistantDesignEntity", synonym, true);
    }

    SECTION("Design entities with typo") {
        testConstructor("Variable", synonym, true);
        testConstructor("procdure", synonym, true);
        testConstructor("statement", synonym, true);
    }

    SECTION("Existing design entities") {
        testConstructor("call", synonym, false);
        testConstructor("variable", synonym, false);
        testConstructor("stmt", synonym, false);
    }
}

TEST_CASE("Declaration: test equals") {
    auto testEquals = [](Declaration first, Declaration second, bool expected) {
        REQUIRE((first == second) == expected);
    };

    // Given
    Declaration varA1 = Declaration("variable", "a");
    Declaration varA2 = Declaration("variable", "a");
    Declaration varB = Declaration("variable", "b");
    Declaration stmtB = Declaration("stmt", "b");

    // Tests
    SECTION("Same design entity and synonym") {
        testEquals(varA1, varA1, true);
        testEquals(varA1, varA2, true);
    }

    SECTION("Different synonym") {
        testEquals(varA1, varB, false);
    }

    SECTION("Different design entity") {
        testEquals(varB, stmtB, false);
    }

}