#include "catch.hpp"
#include <string>
#include <list>
#include <qps/query_parser/PQLError.h>
#include <qps/query_parser/DeclarationParser.h>
#include <qps/query_parser/DeclarationParser.cpp>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("DeclarationParser: test parseOneDeclarationNoError") {
    auto testParseOneDeclarationNoError = [](PQLToken designEntityToken, list<PQLToken> tokens, list<Declaration> expected) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        list<Declaration> actual = parser.parseOneDeclaration(designEntityToken);
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


    SECTION("One synonym") {
        testParseOneDeclarationNoError(PQLToken("variable", PQLTokenType::NAME), 
            list<PQLToken>{PQLToken("v1", PQLTokenType::NAME),PQLToken(";", PQLTokenType::DELIMITER)},
            list<Declaration>{Declaration("variable", "v1")});
    }

    SECTION("Multiple synonyms") {
        testParseOneDeclarationNoError(PQLToken("variable", PQLTokenType::NAME), 
            list<PQLToken>{
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("v2", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER)},
            list<Declaration>{Declaration("variable", "v1"),
                Declaration("variable", "v2")});
        testParseOneDeclarationNoError(PQLToken("variable", PQLTokenType::NAME), 
            list<PQLToken>{
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("v2", PQLTokenType::NAME),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("v3", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER)},
            list<Declaration>{Declaration("variable", "v1"),
                Declaration("variable", "v2"),
                Declaration("variable", "v3")});
    }
}


TEST_CASE("DeclarationParser: test parseOneDeclarationWithError") {
    auto testParseOneDeclarationWithError = [](PQLToken designEntityToken, list<PQLToken> tokens) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // then
        REQUIRE_THROWS_AS(parser.parseOneDeclaration(designEntityToken), PQLError);
    };

    // missing semicolon
    testParseOneDeclarationWithError(PQLToken("variable", PQLTokenType::NAME),
        list<PQLToken>{PQLToken("v1", PQLTokenType::NAME)});
    // missing synonym
    testParseOneDeclarationWithError(PQLToken("variable", PQLTokenType::NAME),
        list<PQLToken>{PQLToken(";", PQLTokenType::DELIMITER)});
    // missing synonym after comma
    testParseOneDeclarationWithError(PQLToken("variable", PQLTokenType::NAME),
        list<PQLToken>{PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken(";", PQLTokenType::DELIMITER)});
    // typo - double comma
    testParseOneDeclarationWithError(PQLToken("variable", PQLTokenType::NAME),
        list<PQLToken>{PQLToken("v1", PQLTokenType::NAME),
        PQLToken(",", PQLTokenType::DELIMITER),
        PQLToken(",", PQLTokenType::DELIMITER),
        PQLToken("v2", PQLTokenType::NAME),
        PQLToken(";", PQLTokenType::DELIMITER)});
}


TEST_CASE("DeclarationParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens, list<Declaration> expected) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        list<Declaration> actual = parser.parse();
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
        testParseNoError(list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER),
                PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            list<Declaration>{Declaration("variable", "v1")});
    }

    SECTION("Multiple declarations") {
        testParseNoError(list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER),
                PQLToken("stmt", PQLTokenType::NAME),
                PQLToken("s1", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER),
                PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            list<Declaration>{Declaration("variable", "v1"), Declaration("stmt", "s1")});
        testParseNoError(list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER),
                PQLToken("stmt", PQLTokenType::NAME),
                PQLToken("s1", PQLTokenType::NAME),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("s2", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER),
                PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            list<Declaration>{Declaration("variable", "v1"), 
                Declaration("stmt", "s1"), 
                Declaration("stmt", "s2")});
    }
}

TEST_CASE("DeclarationParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // then
        REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };


    // query ends without select clause
    testParseWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER)});

    // repeated declarations
    testParseWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
        PQLToken("v1", PQLTokenType::NAME),
        PQLToken(",", PQLTokenType::DELIMITER),
        PQLToken("v1", PQLTokenType::NAME),
        PQLToken(";", PQLTokenType::DELIMITER)});
}