#include "catch.hpp"
#include <qps/query/clause/AffectsClause.h>
#include <qps/query/clause/AffectsTClause.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_parser/parsers/AffectsParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("AffectsParser: test parseAffectsNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        AffectsClause expected) {
            // given
            AffectsParser parser = AffectsParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
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

    AffectsClause expected = AffectsClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createAssignArg("a1"));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("1 synonym, 1 line number") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("a"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createIntegerToken("1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"a", ArgumentType::ASSIGN}
        };

        expected = AffectsClause(
            ClauseArgument::createAssignArg("a"),
            ClauseArgument::createLineNumberArg("1"));

        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("1 line number, 1 wildcard") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createIntegerToken("4"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = AffectsClause(
            ClauseArgument::createLineNumberArg("4"),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("1 wildcard, 1 synonym") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("ss"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"ss", ArgumentType::STMT}
        };

        expected = AffectsClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createStmtArg("ss"));

        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("2 wildcards") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = AffectsClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
}

TEST_CASE("AffectsParser: test parseAffectsTNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        AffectsTClause expected) {
            // given
            AffectsParser parser = AffectsParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("*"),
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

    AffectsTClause expected = AffectsTClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createAssignArg("a1"));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("1 synonym, 1 line number") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("a"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createIntegerToken("1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"a", ArgumentType::ASSIGN}
        };

        expected = AffectsTClause(
            ClauseArgument::createAssignArg("a"),
            ClauseArgument::createLineNumberArg("1"));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 line number, 1 wildcard") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createIntegerToken("4"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = AffectsTClause(
            ClauseArgument::createLineNumberArg("4"),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 wildcard, 1 synonym") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("ss"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"ss", ArgumentType::STMT}
        };

        expected = AffectsTClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createStmtArg("ss"));

        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("2 wildcards") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("*"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("_"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = AffectsTClause(
            ClauseArgument::createWildcardArg(),
            ClauseArgument::createWildcardArg());

        testParseNoError(tokensList, declarationsMap, expected);
    }
}


TEST_CASE("AffectsParser: test parseWithSemanticError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            AffectsParser parser = AffectsParser(tokens, declarations);

            // when
            parser.parse();

            // then
            REQUIRE(!parser.isSemanticallyValid());
    };

    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
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
            PQLToken::createNameToken("Affects"),
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
            PQLToken::createNameToken("Affects"),
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

    SECTION("Illegal arguments: read") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("rr"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"rr", ArgumentType::READ}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: if") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("i1"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"i1", ArgumentType::IF}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: while") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("w"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"w", ArgumentType::WHILE}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: print") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("pp"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"pp", ArgumentType::PRINT}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: call") {
        tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("c"),
            PQLToken::createDelimiterToken(")")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"c", ArgumentType::CALL}
        };

        testParseWithError(tokensList, declarationsMap);
    }
}

TEST_CASE("AffectsParser: test parseWithSyntaxError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            AffectsParser parser = AffectsParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
    };

    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("Affects"),
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
            PQLToken::createNameToken("Affects"),
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
            PQLToken::createNameToken("Affects"),
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