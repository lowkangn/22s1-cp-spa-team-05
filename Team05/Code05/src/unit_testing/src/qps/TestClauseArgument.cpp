#include "catch.hpp"
#include <list>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/ClauseArgument.cpp>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ClauseArgument: test isSynonym") {
    auto testIsSynonym = [](ClauseArgument arg, bool expected) {
        // when
        bool actual = arg.isSynonym();

        // then
        REQUIRE(actual == expected);
    };

    SECTION("Is a synonym") {
        testIsSynonym(ClauseArgument("s", ArgumentType::STMT), true);
        testIsSynonym(ClauseArgument("r", ArgumentType::READ), true);
        testIsSynonym(ClauseArgument("p", ArgumentType::PRINT), true);
        testIsSynonym(ClauseArgument("a", ArgumentType::ASSIGN), true);
        testIsSynonym(ClauseArgument("c", ArgumentType::CALL), true);
        testIsSynonym(ClauseArgument("w", ArgumentType::WHILE), true);
        testIsSynonym(ClauseArgument("i", ArgumentType::IF), true);
        testIsSynonym(ClauseArgument("p", ArgumentType::PROCEDURE), true);
        testIsSynonym(ClauseArgument("v", ArgumentType::VARIABLE), true);
        testIsSynonym(ClauseArgument("c", ArgumentType::CONSTANT), true);
    }

    SECTION("Is not a synonym") {
        testIsSynonym(ClauseArgument("1", ArgumentType::LINE_NUMBER), false);
        testIsSynonym(ClauseArgument("x", ArgumentType::STRING_LITERAL), false);
        testIsSynonym(ClauseArgument("_", ArgumentType::WILDCARD), false);
    }
}

TEST_CASE("ClauseArgument: test isStmtRefNoWildcard") {
    auto testIsStmtRefNoWildcard = [](ClauseArgument arg, bool expected) {
        // when
        bool actual = arg.isStmtRefNoWildcard();

        // then
        REQUIRE(actual == expected);
    };

    SECTION("Is a stmtRef") {

        testIsStmtRefNoWildcard(ClauseArgument("s1", ArgumentType::STMT), true);
        testIsStmtRefNoWildcard(ClauseArgument("r1", ArgumentType::READ), true);
        testIsStmtRefNoWildcard(ClauseArgument("p1", ArgumentType::PRINT), true);
        testIsStmtRefNoWildcard(ClauseArgument("a1", ArgumentType::ASSIGN), true);
        testIsStmtRefNoWildcard(ClauseArgument("c1", ArgumentType::CALL), true);
        testIsStmtRefNoWildcard(ClauseArgument("w1", ArgumentType::WHILE), true);
        testIsStmtRefNoWildcard(ClauseArgument("i1", ArgumentType::IF), true);
        testIsStmtRefNoWildcard(ClauseArgument("2", ArgumentType::LINE_NUMBER), true);
    }

    SECTION("Is not a stmtRef") {
        testIsStmtRefNoWildcard(ClauseArgument("p1", ArgumentType::PROCEDURE), false);
        testIsStmtRefNoWildcard(ClauseArgument("v1", ArgumentType::VARIABLE), false);
        testIsStmtRefNoWildcard(ClauseArgument("c1", ArgumentType::CONSTANT), false);
        testIsStmtRefNoWildcard(ClauseArgument("y", ArgumentType::STRING_LITERAL), false);
        testIsStmtRefNoWildcard(ClauseArgument("_", ArgumentType::WILDCARD), false);
    }
}

TEST_CASE("ClauseArgument: test isEntRefNoWildcard") {
    auto testIsEntRefNoWildcard = [](ClauseArgument arg, bool expected) {
        // when
        bool actual = arg.isEntRefNoWildcard();

        // then
        REQUIRE(actual == expected);
    };

    SECTION("Is an entRef") {
        testIsEntRefNoWildcard(ClauseArgument("p2", ArgumentType::PROCEDURE), true);
        testIsEntRefNoWildcard(ClauseArgument("v2", ArgumentType::VARIABLE), true);
        testIsEntRefNoWildcard(ClauseArgument("c2", ArgumentType::CONSTANT), true);
        testIsEntRefNoWildcard(ClauseArgument("z", ArgumentType::STRING_LITERAL), true);
    }

    SECTION("Is not an entRef") {
        testIsEntRefNoWildcard(ClauseArgument("s2", ArgumentType::STMT), false);
        testIsEntRefNoWildcard(ClauseArgument("r2", ArgumentType::READ), false);
        testIsEntRefNoWildcard(ClauseArgument("p2", ArgumentType::PRINT), false);
        testIsEntRefNoWildcard(ClauseArgument("a2", ArgumentType::ASSIGN), false);
        testIsEntRefNoWildcard(ClauseArgument("c2", ArgumentType::CALL), false);
        testIsEntRefNoWildcard(ClauseArgument("w2", ArgumentType::WHILE), false);
        testIsEntRefNoWildcard(ClauseArgument("i2", ArgumentType::IF), false);
        testIsEntRefNoWildcard(ClauseArgument("3", ArgumentType::LINE_NUMBER), false);
        testIsEntRefNoWildcard(ClauseArgument("_", ArgumentType::WILDCARD), false);
    }
}

TEST_CASE("ClauseArgument: test getLineNumberNoError") {
    auto testGetLineNumberNoError = [](ClauseArgument arg, int expected) {

        // when
        int actual = arg.getLineNumber();

        // then
        REQUIRE(actual == expected);

    };

    ClauseArgument arg = ClauseArgument("4", ArgumentType::LINE_NUMBER);
    testGetLineNumberNoError(arg, 4);
}

TEST_CASE("ClauseArgument: test getLineNumberWithError") {
    auto testGetLineNumberWithError = [](ClauseArgument arg) {

        // then
        REQUIRE_THROWS_AS(arg.getLineNumber(), PQLError);

    };

    ClauseArgument variableArg = ClauseArgument("v", ArgumentType::VARIABLE);
    ClauseArgument stmtArg = ClauseArgument("s", ArgumentType::STMT);
    ClauseArgument readArg = ClauseArgument("r", ArgumentType::READ);
    ClauseArgument printArg = ClauseArgument("p", ArgumentType::PRINT);
    ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
    ClauseArgument callArg = ClauseArgument("c", ArgumentType::CALL);
    ClauseArgument whileArg = ClauseArgument("w", ArgumentType::WHILE);
    ClauseArgument ifArg = ClauseArgument("i", ArgumentType::IF);
    ClauseArgument procArg = ClauseArgument("p", ArgumentType::PROCEDURE);
    ClauseArgument constArg = ClauseArgument("4", ArgumentType::CONSTANT);
    ClauseArgument stringLitArg = ClauseArgument("x", ArgumentType::STRING_LITERAL);
    ClauseArgument wildcardArg = ClauseArgument("_", ArgumentType::WILDCARD);

    list<ClauseArgument> arguments = list<ClauseArgument>{variableArg, procArg, stmtArg, readArg, printArg, assignArg,
                                                          callArg, whileArg, ifArg, constArg,  stringLitArg, wildcardArg};

    for (ClauseArgument argument : arguments) {
        testGetLineNumberWithError(argument);
    }
}

TEST_CASE("ClauseArgument: test getIdentifierNoError") {
    auto testGetIdentifierNoError = [](ClauseArgument arg, string expected) {

        // when
        string actual = arg.getIdentifier();

        // then
        REQUIRE(actual == expected);

    };

    ClauseArgument variableArg = ClauseArgument("v", ArgumentType::VARIABLE);
    testGetIdentifierNoError(variableArg, "v");

    ClauseArgument stmtArg = ClauseArgument("s", ArgumentType::STMT);
    testGetIdentifierNoError(stmtArg, "s");

    ClauseArgument readArg = ClauseArgument("r", ArgumentType::READ);
    testGetIdentifierNoError(readArg, "r");

    ClauseArgument printArg = ClauseArgument("p", ArgumentType::PRINT);
    testGetIdentifierNoError(printArg, "p");

    ClauseArgument assignArg = ClauseArgument("a", ArgumentType::ASSIGN);
    testGetIdentifierNoError(assignArg, "a");

    ClauseArgument callArg = ClauseArgument("c", ArgumentType::CALL);
    testGetIdentifierNoError(callArg, "c");

    ClauseArgument whileArg = ClauseArgument("w", ArgumentType::WHILE);
    testGetIdentifierNoError(whileArg, "w");

    ClauseArgument ifArg = ClauseArgument("i", ArgumentType::IF);
    testGetIdentifierNoError(ifArg, "i");

    ClauseArgument procArg = ClauseArgument("p", ArgumentType::PROCEDURE);
    testGetIdentifierNoError(procArg, "p");

    ClauseArgument constArg = ClauseArgument("4", ArgumentType::CONSTANT);
    testGetIdentifierNoError(constArg, "4");

    ClauseArgument stringLitArg = ClauseArgument("x", ArgumentType::STRING_LITERAL);
    testGetIdentifierNoError(stringLitArg, "x");

    ClauseArgument wildcardArg = ClauseArgument("_", ArgumentType::WILDCARD);
    testGetIdentifierNoError(wildcardArg, "_");

}

TEST_CASE("ClauseArgument: test getIdentifierWithError") {
    auto testGetIdentifierWithError = [](ClauseArgument arg) {

        // then
        REQUIRE_THROWS_AS(arg.getIdentifier(), PQLError);

    };

    ClauseArgument arg = ClauseArgument("4", ArgumentType::LINE_NUMBER);
    testGetIdentifierWithError(arg);

}

TEST_CASE("ClauseArgument: test operator==") {
    auto testOperatorEquals = [](ClauseArgument first, ClauseArgument second, bool expected) {
        // when
        bool actual = (first == second);

        // then
        REQUIRE(actual == expected);
    };

    ClauseArgument variableArg = ClauseArgument("v", ArgumentType::VARIABLE);
    ClauseArgument variableArgAgain = ClauseArgument("v", ArgumentType::VARIABLE);
    ClauseArgument variableArgDiff = ClauseArgument("v1", ArgumentType::VARIABLE);

    SECTION("Same ArgumentType") {
        testOperatorEquals(variableArg, variableArgAgain, true);
        testOperatorEquals(variableArg, variableArgDiff, false);
        testOperatorEquals(variableArgAgain, variableArgDiff, false);
    }

    ClauseArgument stmtArg = ClauseArgument("v", ArgumentType::STMT);
    ClauseArgument readArg = ClauseArgument("v", ArgumentType::READ);
    ClauseArgument printArg = ClauseArgument("v", ArgumentType::PRINT);
    ClauseArgument assignArg = ClauseArgument("v", ArgumentType::ASSIGN);
    ClauseArgument callArg = ClauseArgument("v", ArgumentType::CALL);
    ClauseArgument whileArg = ClauseArgument("v", ArgumentType::WHILE);
    ClauseArgument ifArg = ClauseArgument("v", ArgumentType::IF);
    ClauseArgument procArg = ClauseArgument("v", ArgumentType::PROCEDURE);
    ClauseArgument constArg = ClauseArgument("v", ArgumentType::CONSTANT);
    ClauseArgument lineNumArg = ClauseArgument("v", ArgumentType::LINE_NUMBER);
    ClauseArgument stringLitArg = ClauseArgument("v", ArgumentType::STRING_LITERAL);
    ClauseArgument wildcardArg = ClauseArgument("v", ArgumentType::WILDCARD);

    list<ClauseArgument> arguments = list<ClauseArgument>{variableArg, procArg, stmtArg, readArg, printArg, assignArg,
                                                          callArg, whileArg, ifArg, constArg, lineNumArg, stringLitArg,
                                                          wildcardArg};

    SECTION("Different ArgumentTypes") {
        list<ClauseArgument>::iterator argIter = arguments.begin();
        for (; argIter != arguments.end(); argIter++) {
            list<ClauseArgument>::iterator otherArgIter = next(argIter);
            for (; otherArgIter != arguments.end(); otherArgIter++) {
                testOperatorEquals(*argIter, *otherArgIter, false);
            }
        }
    }
}
