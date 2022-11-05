#include "catch.hpp"
#include <qps/query/clause/NextClause.h>
#include <qps/query/clause/NextTClause.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_parser/parsers/NextParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("NextParser: test parseNextNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        NextClause expected) {
            // given
            NextParser parser = NextParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("a1"),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"s1", ArgumentType::STMT},
        {"a1", ArgumentType::ASSIGN}
    };

    NextClause expected = NextClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createAssignArg("a1"));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 synonym, 1 line number") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("w1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createIntegerToken("1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"w1", ArgumentType::WHILE}
        };

        expected = NextClause(
            ClauseArgument::createWhileArg("w1"),
            ClauseArgument::createLineNumberArg("1"));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 line number, 1 wildcard") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createIntegerToken("4"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"w1", ArgumentType::WHILE}
        };

        expected = NextClause(
            ClauseArgument::createLineNumberArg("4"),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 wildcard, 1 synonym") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("r1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"r1", ArgumentType::READ}
        };

        expected = NextClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createReadArg("r1"));

        testParseNoError(tokensList, declarationsMap, expected);
    }
}

TEST_CASE("NextParser: test parseNextTNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        NextTClause expected) {
            // given
            NextParser parser = NextParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("i1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("p1"),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"i1", ArgumentType::IF},
        {"p1", ArgumentType::PRINT}
    };

    NextTClause expected = NextTClause(
        ClauseArgument::createIfArg("i1"),
        ClauseArgument::createPrintArg("p1"));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("non-container as first argument") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("a1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"a1", ArgumentType::ASSIGN}
        };

        expected = NextTClause(
            ClauseArgument::createAssignArg("a1"),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("2 wildcards") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = NextTClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
}


TEST_CASE("NextParser: test parseWithSemanticError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            NextParser parser = NextParser(tokens, declarations);

            // when
            parser.parse();

            // then
            REQUIRE(!parser.isSemanticallyValid());
    };

    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE},
        {"s1", ArgumentType::STMT}
    };
    SECTION("Illegal arguments: variable") {
        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: procedure") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("proc1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"proc1", ArgumentType::PROCEDURE}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: constant") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("c"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"c", ArgumentType::CONSTANT}
        };

        testParseWithError(tokensList, declarationsMap);
    }
}

TEST_CASE("NextParser: test parseWithSyntaxError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            NextParser parser = NextParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
    };

    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createNameToken("someString"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"s1", ArgumentType::STMT}
    };

    SECTION("Illegal arguments: name string") {
        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: number string") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Next"),
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
            PQLToken::createNameToken("Next"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createIntegerToken("1"),
            PQLToken::createDelimiterToken(")")
        };
        testParseWithError(tokensList, declarationsMap);
    }
}
