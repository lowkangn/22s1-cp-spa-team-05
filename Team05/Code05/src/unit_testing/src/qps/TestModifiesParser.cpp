#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesParser: test parseModifiesSNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        ModifiesSClause expected) {
            // given
            ModifiesParser parser = ModifiesParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };

    list<PQLToken> tokensList = list<PQLToken>{
        PQLToken("Modifies", PQLTokenType::NAME),
        PQLToken("(", PQLTokenType::DELIMITER),
        PQLToken("s1", PQLTokenType::NAME),
        PQLToken(",", PQLTokenType::DELIMITER),
        PQLToken("v1", PQLTokenType::NAME),
        PQLToken(")", PQLTokenType::DELIMITER)
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE},
        {"s1", ArgumentType::STMT}
    };

    ModifiesSClause expected = ModifiesSClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createVariableArg("v1"));

    testParseNoError(tokensList, declarationsMap, expected);

    tokensList = list<PQLToken>{
        PQLToken("Modifies", PQLTokenType::NAME),
        PQLToken("(", PQLTokenType::DELIMITER),
        PQLToken("1", PQLTokenType::INTEGER),
        PQLToken(",", PQLTokenType::DELIMITER),
        PQLToken("\"", PQLTokenType::DELIMITER),
        PQLToken("x", PQLTokenType::NAME),
        PQLToken("\"", PQLTokenType::DELIMITER),
        PQLToken(")", PQLTokenType::DELIMITER)
    };

    expected = ModifiesSClause(
        ClauseArgument::createLineNumberArg("1"),
        ClauseArgument::createStringLiteralArg("x"));

    testParseNoError(tokensList,unordered_map<string, ArgumentType>{}, expected);
}

TEST_CASE("ModifiesParser: test parseModifiesPNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        ModifiesPClause expected) {
            // given
            ModifiesParser parser = ModifiesParser(tokens, declarations);

            // when
            shared_ptr<RelationshipClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };

    list<PQLToken> tokensList = list<PQLToken>{
        PQLToken("Modifies", PQLTokenType::NAME),
        PQLToken("(", PQLTokenType::DELIMITER),
        PQLToken("\"", PQLTokenType::DELIMITER),
        PQLToken("x", PQLTokenType::NAME),
        PQLToken("\"", PQLTokenType::DELIMITER),
        PQLToken(",", PQLTokenType::DELIMITER),
        PQLToken("v1", PQLTokenType::NAME),
        PQLToken(")", PQLTokenType::DELIMITER)
    };

    unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE}
    };

    ModifiesPClause expected = ModifiesPClause(
        ClauseArgument::createStringLiteralArg("x"),
        ClauseArgument::createVariableArg("v1"));

    testParseNoError(tokensList, declarationsMap, expected);
}


TEST_CASE("ModifiesParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            ModifiesParser parser = ModifiesParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };

    SECTION("Undeclared / misspelled synonym") {

        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER)
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithError(tokensList, declarationsMap);

        tokensList = list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE},
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);
    }

    SECTION("Illegal arguments") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("_", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);

        tokensList = list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("s2", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"s2", ArgumentType::STMT},
            {"s1", ArgumentType::STMT}
        };

        testParseWithError(tokensList, declarationsMap);
    }

}
