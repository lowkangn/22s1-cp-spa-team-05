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
    PQLToken stmtNumStmtOnly = PQLToken::createNameToken("stmt");
    PQLToken stmtNum = PQLToken::createKeywordOnlyToken("stmt#");
    PQLToken varName = PQLToken::createNameToken("varName");
    PQLToken procName = PQLToken::createNameToken("procName");

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

    ClauseArgument s1StmtNumArg = ClauseArgument::createStmtNumAttributeArg(s1Arg);
    ClauseArgument a1StmtNumArg = ClauseArgument::createStmtNumAttributeArg(a1Arg);
    ClauseArgument i1StmtNumArg = ClauseArgument::createStmtNumAttributeArg(i1Arg);
    ClauseArgument w1StmtNumArg = ClauseArgument::createStmtNumAttributeArg(w1Arg);
    ClauseArgument r1StmtNumArg = ClauseArgument::createStmtNumAttributeArg(r1Arg);
    ClauseArgument call1StmtNumArg = ClauseArgument::createStmtNumAttributeArg(call1Arg);
    ClauseArgument print1StmtNumArg = ClauseArgument::createStmtNumAttributeArg(print1Arg);

    ClauseArgument call1ProcNameArg = ClauseArgument::createProcNameAttributeArg(call1Arg);
    ClauseArgument r1VarNameArg = ClauseArgument::createVarNameAttributeArg(r1Arg);
    ClauseArgument print1VarNameArg = ClauseArgument::createVarNameAttributeArg(print1Arg);

    // synonyms - entities
    PQLToken proc1 = PQLToken::createNameToken("proc1");
    PQLToken const1 = PQLToken::createNameToken("const1");
    PQLToken v1 = PQLToken::createNameToken("v1");
    ClauseArgument proc1Arg = ClauseArgument::createProcedureArg("proc1");
    ClauseArgument const1Arg = ClauseArgument::createConstantArg("const1");
    ClauseArgument v1Arg = ClauseArgument::createVariableArg("v1");

    ClauseArgument const1ValueArg = ClauseArgument::createValueAttributeArg(const1Arg);
    ClauseArgument proc1ProcNameArg = ClauseArgument::createProcNameAttributeArg(proc1Arg);
    ClauseArgument v1VarNameArg = ClauseArgument::createVarNameAttributeArg(v1Arg);

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
    SECTION("One exact arg (default attribute)") {
        //constants
        tokens = list<PQLToken>{
            one, equals, const1, dot, value,
        };
        nonExactArgs = { const1Arg, const1ValueArg };
        expected = make_shared<WithOneExactClause>(oneArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           const1, dot, value, equals, two,
        };
        nonExactArgs = { const1Arg, const1ValueArg };
        expected = make_shared<WithOneExactClause>(twoArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        //statements
        tokens = list<PQLToken>{
            one, equals, s1, dot, stmtNum, 
        };
        nonExactArgs = { s1Arg, s1StmtNumArg };
        expected = make_shared<WithOneExactClause>(oneArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           a1, dot, stmtNum, equals, two,
        };
        nonExactArgs = { a1Arg, a1StmtNumArg };
        expected = make_shared<WithOneExactClause>(twoArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        //variables and procedures
        tokens = list<PQLToken>{
            quotationMark, someName, quotationMark, equals, v1, dot, varName,
        };
        nonExactArgs = { v1Arg, v1VarNameArg };
        expected = make_shared<WithOneExactClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           v1, dot, varName, equals, quotationMark, someName, quotationMark,
        };
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
            quotationMark, otherName, quotationMark, equals, proc1, dot, procName,
        };
        nonExactArgs = { proc1Arg, proc1ProcNameArg };
        expected = make_shared<WithOneExactClause>(otherNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("One exact arg (non default attributes)") {
        tokens = list<PQLToken>{
            quotationMark, someName, quotationMark, equals, call1, dot, procName,
        };
        nonExactArgs = { call1Arg, call1ProcNameArg };
        expected = make_shared<WithOneExactClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);
    
        tokens = list<PQLToken>{
           r1, dot, varName, equals, quotationMark, someName, quotationMark,
        };
        nonExactArgs = { r1Arg, r1VarNameArg };
        expected = make_shared<WithOneExactClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           print1, dot, varName, equals, quotationMark, someName, quotationMark,
        };
        nonExactArgs = { print1Arg, print1VarNameArg };
        expected = make_shared<WithOneExactClause>(someNameArg, nonExactArgs);
        testParseNoError(tokens, declarations, expected);

    }

    vector<ClauseArgument> lhsArgs;
    vector<ClauseArgument> rhsArgs;
    SECTION("Both non-exact args - (same lhs and rhs, default attributes)") {
        tokens = list<PQLToken>{
             v1, dot, varName, equals, v1, dot, varName,
        };
        lhsArgs = { v1Arg, v1VarNameArg };
        rhsArgs = { v1Arg, v1VarNameArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
            proc1, dot, procName, equals, proc1, dot, procName,
        };
        lhsArgs = { proc1Arg, proc1ProcNameArg };
        rhsArgs = { proc1Arg, proc1ProcNameArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
            const1, dot, value, equals, const1, dot, value,
        };
        lhsArgs = { const1Arg, const1ValueArg };
        rhsArgs = { const1Arg, const1ValueArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
            call1, dot, stmtNum, equals, call1, dot, stmtNum,
        };
        lhsArgs = { call1Arg, call1StmtNumArg };
        rhsArgs = { call1Arg, call1StmtNumArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           r1, dot, stmtNum, equals, r1, dot, stmtNum,
        };
        lhsArgs = { r1Arg, r1StmtNumArg };
        rhsArgs = { r1Arg, r1StmtNumArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           print1, dot, stmtNum, equals, print1, dot, stmtNum,
        };
        lhsArgs = { print1Arg, print1StmtNumArg };
        rhsArgs = { print1Arg, print1StmtNumArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Both non-exact args - (same lhs and rhs, non-default attributes)") {
        tokens = list<PQLToken>{
            call1, dot, procName, equals, call1, dot, procName,
        };
        lhsArgs = { call1Arg, call1ProcNameArg };
        rhsArgs = { call1Arg, call1ProcNameArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           r1, dot, varName, equals, r1, dot, varName,
        };
        lhsArgs = { r1Arg, r1VarNameArg };
        rhsArgs = { r1Arg, r1VarNameArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           print1, dot, varName, equals, print1, dot, varName,
        };
        lhsArgs = { print1Arg, print1VarNameArg };
        rhsArgs = { print1Arg, print1VarNameArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Both non-exact args - (different lhs and rhs)") {
        tokens = list<PQLToken>{
            proc1, dot, procName, equals, call1, dot, procName,
        };
        lhsArgs = { proc1Arg, proc1ProcNameArg };
        rhsArgs = { call1Arg, call1ProcNameArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           v1, dot, varName, equals, proc1, dot, procName,
        };
        lhsArgs = { v1Arg, v1VarNameArg };
        rhsArgs = { proc1Arg, proc1ProcNameArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
           s1, dot, stmtNum, equals, const1, dot, value,
        };
        lhsArgs = { s1Arg, s1StmtNumArg };
        rhsArgs = { const1Arg, const1ValueArg };
        expected = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
        testParseNoError(tokens, declarations, expected);
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
            {v1, dot, stmtNum, equals, const1, dot, value},
            {proc1, dot, stmtNum, equals, const1, dot, value},
            {const1, dot, stmtNum, equals, const1, dot, value},
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
            {s1, dot, stmtNum, equals, quotationMark, someName, quotationMark,},
            {v1, dot, varName, equals, two},
            {quotationMark, otherName, quotationMark, equals, i1, dot, stmtNum,},
            {quotationMark, otherName, quotationMark, equals, a1, dot, stmtNum,},
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
            {v1, dot, varName, equals, s1, dot, stmtNum},
            {const1, dot, value, equals, proc1, dot, procName,},
            {proc1, dot, procName, equals, call1, dot, stmtNum},
            {call1, dot, procName, equals, call1, dot, stmtNum},
            {print1, dot, stmtNum, equals, r1, dot, varName},
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
            two, equals, s1, stmtNum
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
            {a1, dot, stmtNumStmtOnly, equals, const1, dot, value},
            {a1, dot, stmtNum, equals, i1, dot, unknownAttribute},
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
            {i1, dot, stmtNum, const1, dot, value},
        };

        for (list<PQLToken> tokens : tokenLists) {
            testParseWithError(tokens, declarations);
        }
    }

    SECTION("Attempting to find attribute of non-synonyn") {
        list<list<PQLToken>> tokenLists = {
           {const1, dot, value, equals, one, dot, value,},
           {one, dot, stmtNum, equals, two},
           {quotationMark, someName, quotationMark, dot, varName, equals, v1, dot, varName,},
           {const1, dot, value, equals, quotationMark, otherName, quotationMark, dot, stmtNum},
           {const1, dot, value, equals, quotationMark, someName, quotationMark, dot, value},
        };

        for (list<PQLToken> tokens : tokenLists) {
            testParseWithError(tokens, declarations);
        }
    }
}