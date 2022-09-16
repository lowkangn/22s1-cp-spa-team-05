#include "catch.hpp"
#include <list>
#include <qps/query/clause/ClauseArgument.h>


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
        testIsSynonym(ClauseArgument::createStmtArg("s"), true);
        testIsSynonym(ClauseArgument::createReadArg("r"), true);
        testIsSynonym(ClauseArgument::createPrintArg("p"), true);
        testIsSynonym(ClauseArgument::createAssignArg("a"), true);
        testIsSynonym(ClauseArgument::createCallArg("c"), true);
        testIsSynonym(ClauseArgument::createWhileArg("w"), true);
        testIsSynonym(ClauseArgument::createIfArg("i"), true);
        testIsSynonym(ClauseArgument::createProcedureArg("p"), true);
        testIsSynonym(ClauseArgument::createVariableArg("v"), true);
        testIsSynonym(ClauseArgument::createConstantArg("c"), true);
    }

    SECTION("Is not a synonym") {
        testIsSynonym(ClauseArgument::createLineNumberArg("1"), false);
        testIsSynonym(ClauseArgument::createStringLiteralArg("x"), false);
        testIsSynonym(ClauseArgument::createWildcardArg(), false);
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

        testIsStmtRefNoWildcard(ClauseArgument::createStmtArg("s1"), true);
        testIsStmtRefNoWildcard(ClauseArgument::createReadArg("r1"), true);
        testIsStmtRefNoWildcard(ClauseArgument::createPrintArg("p1"), true);
        testIsStmtRefNoWildcard(ClauseArgument::createAssignArg("a1"), true);
        testIsStmtRefNoWildcard(ClauseArgument::createCallArg("c1"), true);
        testIsStmtRefNoWildcard(ClauseArgument::createWhileArg("w1"), true);
        testIsStmtRefNoWildcard(ClauseArgument::createIfArg("i1"), true);
        testIsStmtRefNoWildcard(ClauseArgument::createLineNumberArg("2"), true);
    }

    SECTION("Is not a stmtRef") {
        testIsStmtRefNoWildcard(ClauseArgument::createProcedureArg("p1"), false);
        testIsStmtRefNoWildcard(ClauseArgument::createVariableArg("v1"), false);
        testIsStmtRefNoWildcard(ClauseArgument::createConstantArg("c1"), false);
        testIsStmtRefNoWildcard(ClauseArgument::createStringLiteralArg("y"), false);
        testIsStmtRefNoWildcard(ClauseArgument::createWildcardArg(), false);
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
        testIsEntRefNoWildcard(ClauseArgument::createProcedureArg("p2"), true);
        testIsEntRefNoWildcard(ClauseArgument::createVariableArg("v2"), true);
        testIsEntRefNoWildcard(ClauseArgument::createConstantArg("c2"), true);
        testIsEntRefNoWildcard(ClauseArgument::createStringLiteralArg("z"), true);
    }

    SECTION("Is not an entRef") {
        testIsEntRefNoWildcard(ClauseArgument::createStmtArg("s2"), false);
        testIsEntRefNoWildcard(ClauseArgument::createReadArg("r2"), false);
        testIsEntRefNoWildcard(ClauseArgument::createPrintArg("p2"), false);
        testIsEntRefNoWildcard(ClauseArgument::createAssignArg("a2"), false);
        testIsEntRefNoWildcard(ClauseArgument::createCallArg("c2"), false);
        testIsEntRefNoWildcard(ClauseArgument::createWhileArg("w2"), false);
        testIsEntRefNoWildcard(ClauseArgument::createIfArg("i2"), false);
        testIsEntRefNoWildcard(ClauseArgument::createLineNumberArg("3"), false);
        testIsEntRefNoWildcard(ClauseArgument::createWildcardArg(), false);
    }
}

TEST_CASE("ClauseArgument: test getLineNumberNoError") {
    auto testGetLineNumberNoError = [](ClauseArgument arg, int expected) {

        // when
        int actual = arg.getLineNumber();

        // then
        REQUIRE(actual == expected);

    };

    ClauseArgument arg = ClauseArgument::createLineNumberArg("4");
    testGetLineNumberNoError(arg, 4);
}

TEST_CASE("ClauseArgument: test getLineNumberWithError") {
    auto testGetLineNumberWithError = [](ClauseArgument arg) {

        // then
        REQUIRE_THROWS_AS(arg.getLineNumber(), PQLError);

    };

    ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
    ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
    ClauseArgument readArg = ClauseArgument::createReadArg("r");
    ClauseArgument printArg = ClauseArgument::createPrintArg("p");
    ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
    ClauseArgument callArg = ClauseArgument::createCallArg("c");
    ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
    ClauseArgument ifArg = ClauseArgument::createIfArg("i");
    ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
    ClauseArgument constArg = ClauseArgument::createConstantArg("4");
    ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("x");
    ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

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

    ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
    testGetIdentifierNoError(variableArg, "v");

    ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
    testGetIdentifierNoError(stmtArg, "s");

    ClauseArgument readArg = ClauseArgument::createReadArg("r");
    testGetIdentifierNoError(readArg, "r");

    ClauseArgument printArg = ClauseArgument::createPrintArg("p");
    testGetIdentifierNoError(printArg, "p");

    ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
    testGetIdentifierNoError(assignArg, "a");

    ClauseArgument callArg = ClauseArgument::createCallArg("c");
    testGetIdentifierNoError(callArg, "c");

    ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
    testGetIdentifierNoError(whileArg, "w");

    ClauseArgument ifArg = ClauseArgument::createIfArg("i");
    testGetIdentifierNoError(ifArg, "i");

    ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
    testGetIdentifierNoError(procArg, "p");

    ClauseArgument constArg = ClauseArgument::createConstantArg("4");
    testGetIdentifierNoError(constArg, "4");

    ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("x");
    testGetIdentifierNoError(stringLitArg, "x");

    ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
    testGetIdentifierNoError(wildcardArg, "_");

}

TEST_CASE("ClauseArgument: test getIdentifierWithError") {
    auto testGetIdentifierWithError = [](ClauseArgument arg) {

        // then
        REQUIRE_THROWS_AS(arg.getIdentifier(), PQLError);

    };

    ClauseArgument arg = ClauseArgument::createLineNumberArg("4");
    testGetIdentifierWithError(arg);

}

TEST_CASE("ClauseArgument: test operator==") {
    auto testOperatorEquals = [](ClauseArgument first, ClauseArgument second, bool expected) {
        // when
        bool actual = (first == second);

        // then
        REQUIRE(actual == expected);
    };

    ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
    ClauseArgument variableArgAgain = ClauseArgument::createVariableArg("v");
    ClauseArgument variableArgDiff = ClauseArgument::createVariableArg("v1");

    SECTION("Same ArgumentType") {
        testOperatorEquals(variableArg, variableArgAgain, true);
        testOperatorEquals(variableArg, variableArgDiff, false);
        testOperatorEquals(variableArgAgain, variableArgDiff, false);
    }

    ClauseArgument stmtArg = ClauseArgument::createStmtArg("v");
    ClauseArgument readArg = ClauseArgument::createReadArg("v");
    ClauseArgument printArg = ClauseArgument::createPrintArg("v");
    ClauseArgument assignArg = ClauseArgument::createAssignArg("v");
    ClauseArgument callArg = ClauseArgument::createCallArg("v");
    ClauseArgument whileArg = ClauseArgument::createWhileArg("v");
    ClauseArgument ifArg = ClauseArgument::createIfArg("v");
    ClauseArgument procArg = ClauseArgument::createProcedureArg("v");
    ClauseArgument constArg = ClauseArgument::createConstantArg("v");
    ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("v");
    ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("v");
    ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

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
