#include "catch.hpp"
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_parser/parsers/UsesParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("UsesParser: test parseUsesSNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        UsesSClause expected) {
            // given
            UsesParser parser = UsesParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };

    list<PQLToken> tokensList = list<PQLToken>{
        PQLToken::createNameToken("Uses"),
        PQLToken::createDelimiterToken("("),
        PQLToken::createNameToken("s1"),
        PQLToken::createDelimiterToken(","),
        PQLToken::createNameToken("v1"),
        PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE},
        {"s1", ArgumentType::STMT}
    };

    UsesSClause expected = UsesSClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createVariableArg("v1"));

    testParseNoError(tokensList, declarationsMap, expected);

    tokensList = list<PQLToken>{
        PQLToken::createNameToken("Uses"),
        PQLToken::createDelimiterToken("("),
        PQLToken::createIntegerToken("1"),
        PQLToken::createDelimiterToken(","),
        PQLToken::createDelimiterToken("\""),
        PQLToken::createNameToken("x"),
        PQLToken::createDelimiterToken("\""),
        PQLToken::createDelimiterToken(")")
    };

    expected = UsesSClause(
        ClauseArgument::createLineNumberArg("1"),
        ClauseArgument::createStringLiteralArg("x"));

    testParseNoError(tokensList, unordered_map<string, ArgumentType>{}, expected);
}

TEST_CASE("UsesParser: test parseUsesPNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        UsesPClause expected) {
            // given
            UsesParser parser = UsesParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };

    list<PQLToken> tokensList = list<PQLToken>{
        PQLToken::createNameToken("Uses"),
        PQLToken::createDelimiterToken("("),
        PQLToken::createDelimiterToken("\""),
        PQLToken::createNameToken("x"),
        PQLToken::createDelimiterToken("\""),
        PQLToken::createDelimiterToken(","),
        PQLToken::createNameToken("v1"),
        PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE}
    };

    UsesPClause expected = UsesPClause(
        ClauseArgument::createStringLiteralArg("x"),
        ClauseArgument::createVariableArg("v1"));

    testParseNoError(tokensList, declarationsMap, expected);
}


TEST_CASE("UsesParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            UsesParser parser = UsesParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };

    SECTION("Undeclared / misspelled synonym") {

        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Uses"),
            PQLToken::createDelimiterToken("(")
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithError(tokensList, declarationsMap);

        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Uses"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE},
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Uses"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(")")
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);

        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Uses"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("s1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);
    }

}
