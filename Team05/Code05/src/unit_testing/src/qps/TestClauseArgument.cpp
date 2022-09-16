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
        testIsSynonym(ClauseArgument::generateStmtArg("s"), true);
        testIsSynonym(ClauseArgument::generateReadArg("r"), true);
        testIsSynonym(ClauseArgument::generatePrintArg("p"), true);
        testIsSynonym(ClauseArgument::generateAssignArg("a"), true);
        testIsSynonym(ClauseArgument::generateCallArg("c"), true);
        testIsSynonym(ClauseArgument::generateWhileArg("w"), true);
        testIsSynonym(ClauseArgument::generateIfArg("i"), true);
        testIsSynonym(ClauseArgument::generateProcedureArg("pr"), true);
        testIsSynonym(ClauseArgument::generateVariableArg("v"), true);
        testIsSynonym(ClauseArgument::generateConstantArg("c"), true);
    }

    SECTION("Is not a synonym") {
        testIsSynonym(ClauseArgument::generateLineNumberArg("1"), false);
        testIsSynonym(ClauseArgument::generateStringLiteralArg("x"), false);
        testIsSynonym(ClauseArgument::generateWildcardArg(), false);
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

		testIsStmtRefNoWildcard(ClauseArgument::generateStmtArg("s"), true);
		testIsStmtRefNoWildcard(ClauseArgument::generateReadArg("r"), true);
		testIsStmtRefNoWildcard(ClauseArgument::generatePrintArg("p"), true);
		testIsStmtRefNoWildcard(ClauseArgument::generateAssignArg("a"), true);
		testIsStmtRefNoWildcard(ClauseArgument::generateCallArg("c"), true);
		testIsStmtRefNoWildcard(ClauseArgument::generateWhileArg("w"), true);
		testIsStmtRefNoWildcard(ClauseArgument::generateIfArg("i"), true);
		testIsStmtRefNoWildcard(ClauseArgument::generateLineNumberArg("2"), true);
    }

    SECTION("Is not a stmtRef") {
       	testIsStmtRefNoWildcard(ClauseArgument::generateProcedureArg("pr"), false);
		testIsStmtRefNoWildcard(ClauseArgument::generateVariableArg("v"), false);
		testIsStmtRefNoWildcard(ClauseArgument::generateConstantArg("c"), false);
		testIsStmtRefNoWildcard(ClauseArgument::generateStringLiteralArg("x"), false);
		testIsStmtRefNoWildcard(ClauseArgument::generateWildcardArg(), false);
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
		testIsEntRefNoWildcard(ClauseArgument::generateProcedureArg("pr"), true);
		testIsEntRefNoWildcard(ClauseArgument::generateVariableArg("v"), true);
		testIsEntRefNoWildcard(ClauseArgument::generateConstantArg("c"), true);
		testIsEntRefNoWildcard(ClauseArgument::generateStringLiteralArg("x"), true);
    }

    SECTION("Is not an entRef") {
		testIsEntRefNoWildcard(ClauseArgument::generateStmtArg("s"), false);
		testIsEntRefNoWildcard(ClauseArgument::generateReadArg("r"), false);
		testIsEntRefNoWildcard(ClauseArgument::generatePrintArg("p"), false);
		testIsEntRefNoWildcard(ClauseArgument::generateAssignArg("a"), false);
		testIsEntRefNoWildcard(ClauseArgument::generateCallArg("c"), false);
		testIsEntRefNoWildcard(ClauseArgument::generateWhileArg("w"), false);
		testIsEntRefNoWildcard(ClauseArgument::generateIfArg("i"), false);
		testIsEntRefNoWildcard(ClauseArgument::generateLineNumberArg("2"), false);
		testIsEntRefNoWildcard(ClauseArgument::generateWildcardArg(), false);
    }
}

TEST_CASE("ClauseArgument: test getLineNumberNoError") {
    auto testGetLineNumberNoError = [](ClauseArgument arg, int expected) {

        // when
        int actual = arg.getLineNumber();

        // then
        REQUIRE(actual == expected);

    };

    ClauseArgument arg = ClauseArgument::generateLineNumberArg("4");
    testGetLineNumberNoError(arg, 4);
}

TEST_CASE("ClauseArgument: test getLineNumberWithError") {
    auto testGetLineNumberWithError = [](ClauseArgument arg) {

        // then
        REQUIRE_THROWS_AS(arg.getLineNumber(), PQLError);

    };

    ClauseArgument stmtArg = ClauseArgument::generateStmtArg("s");
	ClauseArgument assignArg = ClauseArgument::generateAssignArg("a");
	ClauseArgument procArg = ClauseArgument::generateProcedureArg("p");
	ClauseArgument variableArg = ClauseArgument::generateVariableArg("v");
	ClauseArgument constArg = ClauseArgument::generateConstantArg("4");
	ClauseArgument readArg = ClauseArgument::generateReadArg("r");
	ClauseArgument printArg = ClauseArgument::generatePrintArg("pr");
	ClauseArgument callArg = ClauseArgument::generateCallArg("c");
	ClauseArgument whileArg = ClauseArgument::generateWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::generateIfArg("i");
	ClauseArgument stringLitArg = ClauseArgument::generateStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::generateWildcardArg();

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

	ClauseArgument variableArg = ClauseArgument::generateVariableArg("v");
    testGetIdentifierNoError(variableArg, "v");

	ClauseArgument stmtArg = ClauseArgument::generateStmtArg("s");
    testGetIdentifierNoError(stmtArg, "s");

	ClauseArgument readArg = ClauseArgument::generateReadArg("r");
    testGetIdentifierNoError(readArg, "r");

	ClauseArgument printArg = ClauseArgument::generatePrintArg("pr");
    testGetIdentifierNoError(printArg, "pr");

	ClauseArgument assignArg = ClauseArgument::generateAssignArg("a");
    testGetIdentifierNoError(assignArg, "a");

	ClauseArgument callArg = ClauseArgument::generateCallArg("c");
    testGetIdentifierNoError(callArg, "c");

	ClauseArgument whileArg = ClauseArgument::generateWhileArg("w");
    testGetIdentifierNoError(whileArg, "w");

	ClauseArgument ifArg = ClauseArgument::generateIfArg("i");
    testGetIdentifierNoError(ifArg, "i");

	ClauseArgument procArg = ClauseArgument::generateProcedureArg("p");
    testGetIdentifierNoError(procArg, "p");

	ClauseArgument constArg = ClauseArgument::generateConstantArg("4");
    testGetIdentifierNoError(constArg, "4");

	ClauseArgument stringLitArg = ClauseArgument::generateStringLiteralArg("x");
    testGetIdentifierNoError(stringLitArg, "x");

	ClauseArgument wildcardArg = ClauseArgument::generateWildcardArg();
    testGetIdentifierNoError(wildcardArg, "_");

}

TEST_CASE("ClauseArgument: test getIdentifierWithError") {
    auto testGetIdentifierWithError = [](ClauseArgument arg) {

        // then
        REQUIRE_THROWS_AS(arg.getIdentifier(), PQLError);

    };

	ClauseArgument arg = ClauseArgument::generateLineNumberArg("4");
    testGetIdentifierWithError(arg);

}

TEST_CASE("ClauseArgument: test operator==") {
    auto testOperatorEquals = [](ClauseArgument first, ClauseArgument second, bool expected) {
        // when
        bool actual = (first == second);

        // then
        REQUIRE(actual == expected);
    };

    ClauseArgument variableArg = ClauseArgument::generateVariableArg("v");
    ClauseArgument variableArgAgain = ClauseArgument::generateVariableArg("v");
    ClauseArgument variableArgDiff = ClauseArgument::generateVariableArg("v1");

    SECTION("Same ArgumentType") {
        testOperatorEquals(variableArg, variableArgAgain, true);
        testOperatorEquals(variableArg, variableArgDiff, false);
        testOperatorEquals(variableArgAgain, variableArgDiff, false);
    }

	ClauseArgument stmtArg = ClauseArgument::generateStmtArg("s");
	ClauseArgument assignArg = ClauseArgument::generateAssignArg("a");
	ClauseArgument procArg = ClauseArgument::generateProcedureArg("p");
	ClauseArgument constArg = ClauseArgument::generateConstantArg("4");
	ClauseArgument readArg = ClauseArgument::generateReadArg("r");
	ClauseArgument printArg = ClauseArgument::generatePrintArg("pr");
	ClauseArgument callArg = ClauseArgument::generateCallArg("c");
	ClauseArgument whileArg = ClauseArgument::generateWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::generateIfArg("i");
	ClauseArgument lineNumArg = ClauseArgument::generateLineNumberArg("5");
	ClauseArgument stringLitArg = ClauseArgument::generateStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::generateWildcardArg();

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
