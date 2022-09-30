#include "catch.hpp"
#include <string>
#include <list>
#include <qps/exceptions/PQLSyntaxError.h>
#include <qps/exceptions/PQLSemanticError.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <unordered_map>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("DeclarationParser: test parseOneDeclarationNoError") {
    auto testParseOneDeclarationNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> expected) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        parser.parseOneDeclaration();
        unordered_map<string, ArgumentType> actual = parser.getDeclarations();

        // then
        REQUIRE(actual == expected);
    };


    SECTION("One synonym") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";")
        };

        testParseOneDeclarationNoError(tokensList,
            unordered_map<string, ArgumentType>{ {"v1", ArgumentType::VARIABLE}});
    }

    
}


TEST_CASE("DeclarationParser: test parseOneDeclarationWithSyntaxError") {
    auto testParseOneDeclarationWithError = [](list<PQLToken> tokens) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // then
        REQUIRE_THROWS_AS(parser.parseOneDeclaration(), PQLSyntaxError);
    };

    SECTION("Missing semicolon") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1")
        };

        testParseOneDeclarationWithError(tokensList);
    }
    SECTION("Missing synonym") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createDelimiterToken(";")
        };
        testParseOneDeclarationWithError(tokensList);
    }
    SECTION("Missing synonym after comma") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken(";")
        };
        testParseOneDeclarationWithError(tokensList);
    }
    SECTION("Double comma") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v2"),
            PQLToken::createDelimiterToken(";")
        };
        testParseOneDeclarationWithError(tokensList);
    }
}


TEST_CASE("DeclarationParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens, unordered_map<string, ArgumentType> expected) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        unordered_map<string, ArgumentType> actual = parser.parse();

        // then
        REQUIRE(actual == expected);
    };


    SECTION("One declaration") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE}
        };

        testParseNoError(tokensList, declarationsMap);
    }

    SECTION("Multiple declarations") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("stmt"),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            { "s1", ArgumentType::STMT }
        };

        testParseNoError(tokensList, declarationsMap);

        tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("stmt"),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("s2"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            { "s1", ArgumentType::STMT }, { "s2", ArgumentType::STMT }
        };

        testParseNoError(tokensList, declarationsMap);
    }
}

TEST_CASE("DeclarationParser: test parseWithSyntaxError") {
    auto testParseWithError = [](list<PQLToken> tokens) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // then
        REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
    };


    SECTION("No select clause") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";")
        };

        testParseWithError(tokensList);
    }

    SECTION("Missing comma") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createNameToken("v2"),
            PQLToken::createDelimiterToken(";")
        };

        testParseWithError(tokensList);
    }
}

TEST_CASE("DeclarationParser: test parseWithSemanticError") {
    auto testParseWithError = [](list<PQLToken> tokens) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        parser.parse();

        // then
        REQUIRE(!parser.isSemanticallyValid());
    };

    SECTION("Repeated synonym name - same design entity") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1"),
        };

        testParseWithError(tokensList);
    }

    SECTION("Repeated synonym name - different design entities") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("stmt"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1"),
        };

        testParseWithError(tokensList);
    }
}

TEST_CASE("DeclarationParser: test getRemainingTokens (after parsing)") {
    auto testGetRemainingTokens = [](list<PQLToken> tokens, list<PQLToken> expected) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        parser.parse();
        list<PQLToken> actual = parser.getRemainingTokens();
        bool isEqual = actual.size() == expected.size();
        if (isEqual) {
            while (!actual.empty()) {
                isEqual = isEqual && actual.front().equals(expected.front());
                actual.pop_front();
                expected.pop_front();
            }
        }

        // then
        REQUIRE(isEqual);
    };


    SECTION("One declaration") {
        list<PQLToken> initialList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
        };

        list<PQLToken> remainingList = list<PQLToken>{
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
        };

        testGetRemainingTokens(initialList, remainingList);
    }

    SECTION("Multiple declarations; such that and pattern included") {
        list<PQLToken> initialList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("stmt"),
            PQLToken::createNameToken("s1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1"),
            PQLToken::createNameToken("such"),
            PQLToken::createNameToken("that"),
            PQLToken::createNameToken("Modifies"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("v1"),
            PQLToken::createDelimiterToken(")")
        };

        list<PQLToken> remainingList = list<PQLToken>{
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1"),
            PQLToken::createNameToken("such"),
            PQLToken::createNameToken("that"),
            PQLToken::createNameToken("Modifies"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createDelimiterToken("1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("v1"),
            PQLToken::createDelimiterToken(")")
        };

        testGetRemainingTokens(initialList, remainingList);
    }
}