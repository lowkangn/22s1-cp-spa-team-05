#include "catch.hpp"
#include <qps/query/clause/WithClause.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query_parser/parsers/WithParser.h>


using namespace std;

// =============== UNIT TESTS ====================

namespace WithParserTestUtil {

    // operators
    PQLToken equals = PQLToken::createOperatorToken("=");
    PQLToken dot = PQLToken::createOperatorToken(".");

    // attributes
    PQLToken value = PQLToken::createNameToken("value");
    PQLToken stmtNumStmt = PQLToken::createNameToken("stmt");
    PQLToken stmtNumHash = PQLToken::createDelimiterToken("#");
    PQLToken varName = PQLToken::createNameToken("varName");
    PQLToken procName = PQLToken::createNameToken("procName");
    ClauseArgument valueArg = ClauseArgument::createValueAttributeArg();
    ClauseArgument stmtNumArg = ClauseArgument::createStmtNumAttributeArg();
    ClauseArgument varNameArg = ClauseArgument::createVarNameAttributeArg();
    ClauseArgument procNameArg = ClauseArgument::createProcNameAttributeArg();

    // line numbers
    PQLToken one = PQLToken::createIntegerToken("1");
    PQLToken two = PQLToken::createIntegerToken("2");
    ClauseArgument oneArg = ClauseArgument::createLineNumberArg("1");
    ClauseArgument twoArg = ClauseArgument::createLineNumberArg("2");

    // string literals
    PQLToken quotationMark = PQLToken::createDelimiterToken("\"");
    PQLToken someName = PQLToken::createNameToken("someName");
    PQLToken otherName = PQLToken::createNameToken("otherName");
    ClauseArgument someNameArg = ClauseArgument::createStringLiteralArg("someName");
    ClauseArgument otherNameArg = ClauseArgument::createStringLiteralArg("otherName");

    // synonyms - statements
    PQLToken s1 = PQLToken::createNameToken("s1");
    PQLToken a1 = PQLToken::createNameToken("a1");
    PQLToken i1 = PQLToken::createNameToken("i1");
    PQLToken w1 = PQLToken::createNameToken("w1");
    PQLToken r1 = PQLToken::createNameToken("r1");
    PQLToken call1 = PQLToken::createNameToken("call1");
    PQLToken print1 = PQLToken::createNameToken("print1");
    ClauseArgument s1Arg = ClauseArgument::createStmtArg("s1");
    ClauseArgument a1Arg = ClauseArgument::createAssignArg("a1");
    ClauseArgument i1Arg = ClauseArgument::createIfArg("i1");
    ClauseArgument w1Arg = ClauseArgument::createWhileArg("w1");
    ClauseArgument r1Arg = ClauseArgument::createReadArg("r1");
    ClauseArgument call1Arg = ClauseArgument::createCallArg("call1");
    ClauseArgument print1Arg = ClauseArgument::createPrintArg("print1");

    // synonyms - entities
    PQLToken proc1 = PQLToken::createNameToken("proc1");
    PQLToken const1 = PQLToken::createNameToken("const1");
    PQLToken v1 = PQLToken::createNameToken("v1");
    ClauseArgument proc1Arg = ClauseArgument::createProcedureArg("proc1");
    ClauseArgument const1Arg = ClauseArgument::createConstantArg("const1");
    ClauseArgument v1Arg = ClauseArgument::createVariableArg("v1");

    // declarations
    unordered_map<string, ArgumentType> declarations{
        {"s1", ArgumentType::STMT},
        {"a1", ArgumentType::ASSIGN},
        {"i1", ArgumentType::IF},
        {"w1", ArgumentType::WHILE},
        {"r1", ArgumentType::READ},
        {"call1", ArgumentType::CALL},
        {"print1", ArgumentType::PRINT},
        {"proc1", ArgumentType::PROCEDURE},
        {"const1", ArgumentType::CONSTANT},
        {"v1", ArgumentType::VARIABLE}
    };
}

using namespace WithParserTestUtil;

TEST_CASE("WithParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        shared_ptr<WithClause> expected) {
            // given
            WithParser parser = WithParser(tokens, declarations);

            // when
            shared_ptr<WithClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected->equals(actualPtr));
    };

    list<PQLToken> tokens;
    shared_ptr<WithClause> expected;

    SECTION("Both exact args (int)") {
        tokens = list<PQLToken>{
            one, equals, one
        };
        expected = make_shared<WithBothExactClause>(oneArg, oneArg);
        testParseNoError(tokens, declarations, expected);


        tokens = list<PQLToken>{
            one, equals, two
        };
        expected = make_shared<WithBothExactClause>(oneArg, twoArg);
        testParseNoError(tokens, declarations, expected);
    }
    
    SECTION("Both exact args (name)") {
        tokens = list<PQLToken>{
            quotationMark, someName, quotationMark, equals, quotationMark, someName, quotationMark,
        };
        expected = make_shared<WithBothExactClause>(someNameArg, someNameArg);
        testParseNoError(tokens, declarations, expected);


        tokens = list<PQLToken>{
            quotationMark, someName, quotationMark, equals, quotationMark, otherName, quotationMark,
        };
        expected = make_shared<WithBothExactClause>(someNameArg, otherNameArg);
        testParseNoError(tokens, declarations, expected);
    }
    
    vector<ClauseArgument> nonExactArgs;
    SECTION("One exact arg (integers - constants)") {
        tokens = list<PQLToken>{
            one, equals, const1, dot, value,
        };
        nonExactArgs = { const1Arg, valueArg };
        expected = make_shared<WithEntityClause>(oneArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           const1, dot, value, equals, two,
        };
        nonExactArgs = { const1Arg, valueArg };
        expected = make_shared<WithEntityClause>(twoArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("One exact arg (integers - statements)") {

        tokens = list<PQLToken>{
            one, equals, s1, dot, stmtNumStmt, stmtNumHash
        };
        nonExactArgs = { s1Arg, stmtNumArg };
        expected = make_shared<WithEntityClause>(oneArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           a1, dot, stmtNumStmt, stmtNumHash, equals, two,
        };
        nonExactArgs = { a1Arg, stmtNumArg };
        expected = make_shared<WithEntityClause>(twoArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

    }

    SECTION("One exact arg (names - procedures and variables)") {
        tokens = list<PQLToken>{
            quotationMark, someName, quotationMark, equals, v1, dot, varName,
        };
        nonExactArgs = { v1Arg, varNameArg };
        expected = make_shared<WithEntityClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           v1, dot, varName, equals, quotationMark, someName, quotationMark,
        };
        testParseNoError(tokens, declarations, expected);


        tokens = list<PQLToken>{
            quotationMark, otherName, quotationMark, equals, proc1, dot, procName,
        };
        nonExactArgs = { proc1Arg, procNameArg };
        expected = make_shared<WithEntityClause>(otherNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("One exact arg (names - statements)") {

        tokens = list<PQLToken>{
            quotationMark, someName, quotationMark, equals, call1, dot, procName,
        };
        nonExactArgs = { call1Arg, procNameArg };
        expected = make_shared<WithRelationshipClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);
    
        tokens = list<PQLToken>{
           r1, dot, varName, equals, quotationMark, someName, quotationMark,
        };
        nonExactArgs = { r1Arg, varNameArg };
        expected = make_shared<WithRelationshipClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           print1, dot, varName, equals, quotationMark, someName, quotationMark,
        };
        nonExactArgs = { print1Arg, varNameArg };
        expected = make_shared<WithRelationshipClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

    }

    SECTION("Two exact args") {
        //TODO
    }
}

TEST_CASE("WithParser: test parseWithSemanticError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            WithParser parser = WithParser(tokens, declarations);

            // when
            parser.parse();

            // then
            REQUIRE(!parser.isSemanticallyValid());
    };


    SECTION("Undeclared / mispelled synonym") {
        PQLToken undeclaredSynonym = PQLToken::createNameToken("undeclaredSynonym");
        list<PQLToken>tokens = list<PQLToken>{
            undeclaredSynonym, dot, value, equals, one,
        };

        testParseWithError(tokens, declarations);
    }

    SECTION("Mismatch betwwen synonym type and attribute") {
        
        list<list<PQLToken>> tokenLists = {
            {v1, dot, stmtNumStmt, stmtNumHash, equals, const1, dot, value},
            {proc1, dot, stmtNumStmt, stmtNumHash, equals, const1, dot, value},
            {const1, dot, stmtNumStmt, stmtNumHash, equals, const1, dot, value},
            {proc1, dot, varName, equals, const1, dot, value},
            {s1, dot, procName, equals, const1, dot, value},
            {const1, dot, varName, equals, const1, dot, value},
            {const1, dot, value, equals, v1, dot, procName,},
            {const1, dot, value, equals, const1, dot, procName,},
            {const1, dot, value, equals, call1, dot, varName,},
            {const1, dot, value, equals, print1, dot, procName,},
            {const1, dot, value, equals, r1, dot, value,},
            {const1, dot, value, equals, s1, dot, value,},
            {const1, dot, value, equals, a1, dot, varName,},
            {const1, dot, value, equals, i1, dot, varName,},
            {const1, dot, value, equals, w1, dot, varName,},
        };

        for (list<PQLToken> tokens : tokenLists) {
             testParseWithError(tokens, declarations);
        }
    }

    SECTION("Mismatch between the 2 refs (1 attrRef, 1 exact)") {

        list<list<PQLToken>> tokenLists = {
            {quotationMark, someName, quotationMark, equals, const1, dot, value},
            {const1, dot, value, equals, quotationMark, someName, quotationMark,},
            {proc1, dot, procName, equals, one},
            {s1, dot, stmtNumStmt, stmtNumHash, equals, quotationMark, someName, quotationMark,},
            {v1, dot, varName, equals, two},
            {quotationMark, otherName, quotationMark, equals, i1, dot, stmtNumStmt, stmtNumHash,},
            {quotationMark, otherName, quotationMark, equals, a1, dot, stmtNumStmt, stmtNumHash,},
            {call1, dot, procName, equals, two},
            {one, equals, print1, dot, varName},
            {r1, dot, varName, equals, one},
        };

        for (list<PQLToken> tokens : tokenLists) {
            testParseWithError(tokens, declarations);
        }
    }

    SECTION("Mismatch between the 2 refs (2 attrRefs)") {

        list<list<PQLToken>> tokenLists = {
            {v1, dot, varName, equals, const1, dot, value},
            {v1, dot, varName, equals, s1, dot, stmtNumStmt, stmtNumHash},
            {const1, dot, value, equals, proc1, dot, procName,},
            {proc1, dot, procName, equals, call1, dot, stmtNumStmt, stmtNumHash},
            {call1, dot, procName, equals, call1, dot, stmtNumStmt, stmtNumHash},
            {print1, dot, stmtNumStmt, stmtNumHash, equals, r1, dot, varName},
        };

        for (list<PQLToken> tokens : tokenLists) {
            testParseWithError(tokens, declarations);
        }
    }
}

TEST_CASE("WithParser: test parseWithSyntaxError") {

    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            WithParser parser = WithParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
    };

    SECTION("Missing synonym in attrRef") {
        list<PQLToken> tokens = list<PQLToken>{
            one, equals, dot, value,
        };
        testParseWithError(tokens, declarations);

        tokens = list<PQLToken>{
            dot, procName, equals, quotationMark, someName, quotationMark
        };
        testParseWithError(tokens, declarations);
    }

    SECTION("Missing dot in attrRef") {
        list<PQLToken> tokens = list<PQLToken>{
            two, equals, s1, stmtNumStmt, stmtNumHash
        };
        testParseWithError(tokens, declarations);

        tokens = list<PQLToken>{
            v1, varName, equals, quotationMark, otherName, quotationMark
        };
        testParseWithError(tokens, declarations);
    }

    SECTION("Missing / Partially missing / Unknown attribute") {

        PQLToken unknownAttribute = PQLToken::createNameToken("numIters");

        list<list<PQLToken>> tokenLists = {
            {v1, equals, quotationMark, someName, quotationMark,},
            {v1, dot, varName, equals, proc1},
            {one, equals, const1},
            {a1, dot, stmtNumStmt, equals, const1, dot, value},
            {a1, dot, stmtNumStmt, stmtNumHash, equals, i1, dot, unknownAttribute},
        };

        for (list<PQLToken> tokens : tokenLists) {
            testParseWithError(tokens, declarations);
        }
    }

    SECTION("Missing equals") {

        list<list<PQLToken>> tokenLists = {
            {two, one,},
            {quotationMark, someName, quotationMark, v1, dot, varName,},
            {const1, dot, value, one},
            {call1, dot, procName, print1, dot, varName},
            {i1, dot, stmtNumStmt, stmtNumHash, const1, dot, value},
        };

        for (list<PQLToken> tokens : tokenLists) {
            testParseWithError(tokens, declarations);
        }
    }

    SECTION("Attempting to find attribute of non-synonyn") {
        list<list<PQLToken>> tokenLists = {
           {const1, dot, value, equals, one, dot, value,},
           {one, dot, stmtNumStmt, stmtNumHash, equals, two},
           {quotationMark, someName, quotationMark, dot, varName, equals, v1, dot, varName,},
           {const1, dot, value, equals, quotationMark, otherName, quotationMark, dot, stmtNumStmt, stmtNumHash},
           {const1, dot, value, equals, quotationMark, someName, quotationMark, dot, value},
        };

        for (list<PQLToken> tokens : tokenLists) {
            testParseWithError(tokens, declarations);
        }
    }
}