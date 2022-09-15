#include "catch.hpp"
#include <string>
#include <list>
#include <qps/exceptions/PQLError.h>
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
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)
        };

        testParseOneDeclarationNoError(tokensList,
            unordered_map<string, ArgumentType>{ {"v1", ArgumentType::VARIABLE}});
    }

    
}


TEST_CASE("DeclarationParser: test parseOneDeclarationWithError") {
    auto testParseOneDeclarationWithError = [](list<PQLToken> tokens) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // then
        REQUIRE_THROWS_AS(parser.parseOneDeclaration(), PQLError);
    };

    SECTION("Missing semicolon") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
        };

        testParseOneDeclarationWithError(tokensList);
    }
    SECTION("Missing synonym") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)
        };
        testParseOneDeclarationWithError(tokensList);
    }
    SECTION("Missing synonym after comma") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken(";", PQLTokenType::DELIMITER)
        };
        testParseOneDeclarationWithError(tokensList);
    }
    SECTION("Double comma") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v2", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)
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
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE}
        };

        testParseNoError(tokensList, declarationsMap);
    }

    SECTION("Multiple declarations") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("stmt", PQLTokenType::NAME),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
        };

        unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            { "s1", ArgumentType::STMT }
        };

        testParseNoError(tokensList, declarationsMap);

        tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("stmt", PQLTokenType::NAME),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("s2", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
        };

        declarationsMap = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            { "s1", ArgumentType::STMT }, { "s2", ArgumentType::STMT }
        };

        testParseNoError(tokensList, declarationsMap);
    }
}

TEST_CASE("DeclarationParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // then
        REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };


    SECTION("No select clause") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)
        };

        testParseWithError(tokensList);
    }

    SECTION("Missing comma") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken("v2", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)
        };

        testParseWithError(tokensList);
    }

    SECTION("Repeated synonym name - same design entity") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)
        };

        testParseWithError(tokensList);
    }

    SECTION("Repeated synonym name - different design entities") {
        list<PQLToken> tokensList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("stmt", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)
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
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
        };

        list<PQLToken> remainingList = list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
        };

        testGetRemainingTokens(initialList, remainingList);
    }

    SECTION("Multiple declarations; such that and pattern included") {
        list<PQLToken> initialList = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("stmt", PQLTokenType::NAME),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken("such", PQLTokenType::NAME),
            PQLToken("that", PQLTokenType::NAME),
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("1", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        list<PQLToken> remainingList = list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken("such", PQLTokenType::NAME),
            PQLToken("that", PQLTokenType::NAME),
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("1", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        testGetRemainingTokens(initialList, remainingList);
    }
}