#include "catch.hpp"
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_parser/parsers/ParentParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ParentParser: test parseParentNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        ParentClause expected) {
            // given
            ParentParser parser = ParentParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("a1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"s1", ArgumentType::STMT},
        {"a1", ArgumentType::ASSIGN}
    };

    ParentClause expected = ParentClause(
        ClauseArgument("s1", ArgumentType::STMT),
        ClauseArgument("a1", ArgumentType::ASSIGN));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 synonym, 1 line number") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("w1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("1", PQLTokenType::INTEGER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"w1", ArgumentType::WHILE}
        };

        expected = ParentClause(
            ClauseArgument("w1", ArgumentType::WHILE),
            ClauseArgument("1", ArgumentType::LINE_NUMBER));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 line number, 1 wildcard") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("4", PQLTokenType::INTEGER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("_", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"w1", ArgumentType::WHILE}
        };

        expected = ParentClause(
            ClauseArgument("4", ArgumentType::LINE_NUMBER),
            ClauseArgument("_", ArgumentType::WILDCARD));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("1 wildcard, 1 synonym") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("_", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("r1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"r1", ArgumentType::READ}
        };

        expected = ParentClause(
            ClauseArgument("_", ArgumentType::WILDCARD),
            ClauseArgument("r1", ArgumentType::READ));

        testParseNoError(tokensList, declarationsMap, expected);
    }
}

TEST_CASE("ParentParser: test parseParentTNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        ParentTClause expected) {
            // given
            ParentParser parser = ParentParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };
    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("*", PQLTokenType::DELIMITER),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("i1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("p1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"i1", ArgumentType::IF},
        {"p1", ArgumentType::PRINT}
    };

    ParentTClause expected = ParentTClause(
        ClauseArgument("i1", ArgumentType::IF),
        ClauseArgument("p1", ArgumentType::PRINT));


    SECTION("2 synonyms") {
        testParseNoError(tokensList, declarationsMap, expected);
    }

    SECTION("non-container as first argument") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("*", PQLTokenType::DELIMITER),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("a1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("_", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"a1", ArgumentType::ASSIGN}
        };

        expected = ParentTClause(
            ClauseArgument("a1", ArgumentType::ASSIGN),
            ClauseArgument("_", ArgumentType::WILDCARD));

        testParseNoError(tokensList, declarationsMap, expected);
    }
    SECTION("2 wildcards") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("*", PQLTokenType::DELIMITER),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("_", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("_", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{};

        expected = ParentTClause(
            ClauseArgument("_", ArgumentType::WILDCARD),
            ClauseArgument("_", ArgumentType::WILDCARD));

        testParseNoError(tokensList, declarationsMap, expected);
    }
}


TEST_CASE("ParentParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            ParentParser parser = ParentParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };

    list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
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
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("proc1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("s2", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"proc1", ArgumentType::PROCEDURE}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: procedure") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s2", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("c", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"c", ArgumentType::CONSTANT}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: name string") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("*", PQLTokenType::DELIMITER),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken("someString", PQLTokenType::NAME),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments: number string (constant)") {
        tokensList = list<PQLToken>{
            PQLToken("Parent", PQLTokenType::NAME),
            PQLToken("*", PQLTokenType::DELIMITER),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken("12", PQLTokenType::NAME),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);
    }
}
