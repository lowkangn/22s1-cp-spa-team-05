#include "catch.hpp"
#include <qps/query/clause/CallsClause.h>
#include <qps/query/clause/CallsTClause.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_parser/parsers/CallsParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("CallsParser: test parseCallsNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        CallsClause expected) {
            // given
            CallsParser parser = CallsParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("p2"),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"p1", ArgumentType::PROCEDURE},
        {"p2", ArgumentType::PROCEDURE}
    };

    CallsClause expected = CallsClause(
        ClauseArgument::createProcedureArg("p1"),
        ClauseArgument::createProcedureArg("p2"));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 synonym, 1 string literal") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createNameToken("main"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"p1", ArgumentType::PROCEDURE}
        };

        expected = CallsClause(
            ClauseArgument::createProcedureArg("p1"),
            ClauseArgument::createStringLiteralArg("main"));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 wildcard, 1 string literal") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createNameToken("main"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = CallsClause(
            ClauseArgument::createStringLiteralArg("main"),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 wildcard, 1 synonym") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"p1", ArgumentType::PROCEDURE}
        };

        expected = CallsClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createProcedureArg("p1"));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("2 wildcards") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = CallsClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
}

TEST_CASE("CallsParser: test parseCallsTNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        CallsTClause expected) {
            // given
            CallsParser parser = CallsParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("p2"),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"p1", ArgumentType::PROCEDURE},
        {"p2", ArgumentType::PROCEDURE}
    };

    CallsTClause expected = CallsTClause(
        ClauseArgument::createProcedureArg("p1"),
        ClauseArgument::createProcedureArg("p2"));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("1 synonym, 1 string literal") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createNameToken("main"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"p1", ArgumentType::PROCEDURE}
        };

        expected = CallsTClause(
            ClauseArgument::createProcedureArg("p1"),
            ClauseArgument::createStringLiteralArg("main"));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 wildcard, 1 string literal") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createNameToken("main"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = CallsTClause(
            ClauseArgument::createStringLiteralArg("main"),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 wildcard, 1 synonym") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"p1", ArgumentType::PROCEDURE}
        };

        expected = CallsTClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createProcedureArg("p1"));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("2 wildcards") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = CallsTClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
}


TEST_CASE("CallsParser: test parseWithSemanticError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            CallsParser parser = CallsParser(tokens, declarations);

            // when
            parser.parse();

            // then
            REQUIRE(!parser.isSemanticallyValid());
    };

    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE},
        {"p1", ArgumentType::PROCEDURE}
    };
    SECTION("Illegal arguments: variable") {
        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: statement") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s1", ArgumentType::STMT},
            {"p1", ArgumentType::PROCEDURE}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: constant") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("c"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"p1", ArgumentType::PROCEDURE},
            {"c", ArgumentType::CONSTANT}
        };

        testParseWithError(tokensList, declarationsMap);
    }
}


TEST_CASE("CallsParser: test parseWithSyntaxError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            CallsParser parser = CallsParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
    };

    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createIntegerToken("1"),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"p1", ArgumentType::PROCEDURE},
        {"p2", ArgumentType::PROCEDURE},
    };

    SECTION("Illegal arguments: number") {
        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: number string") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createIntegerToken("1"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createDelimiterToken(")")
        };
        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Double *") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Calls"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("p2"),
            PQLToken::createDelimiterToken(")")
        };
        testParseWithError(tokensList, declarationsMap);
    }
}
