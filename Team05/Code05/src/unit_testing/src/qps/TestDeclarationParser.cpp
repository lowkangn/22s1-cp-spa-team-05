#include "catch.hpp"
#include <string>
#include <list>
#include <qps/exceptions/PQLError.h>
#include <qps/query_parser/DesignEntity.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/DeclarationParser.cpp>
#include <unordered_map>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("DeclarationParser: test parseOneDeclarationNoError") {
    auto testParseOneDeclarationNoError = [](list<PQLToken> tokens,
        unordered_map<string, DesignEntity> expected) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        parser.parseOneDeclaration();
        unordered_map<string, DesignEntity> actual = parser.getDeclarations();

        // then
        REQUIRE(actual == expected);
    };


    SECTION("One synonym") {
        testParseOneDeclarationNoError(list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME), 
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)},
            unordered_map<string, DesignEntity>{ {"v1", DesignEntity::VARIABLE}});
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
        testParseOneDeclarationWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)});
    }
    SECTION("Missing synonym") {
        testParseOneDeclarationWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)});
    }
    SECTION("Missing synonym after comma") {
        testParseOneDeclarationWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken(";", PQLTokenType::DELIMITER)});
    }
    SECTION("Double comma") {
        testParseOneDeclarationWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v2", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)});
    }
}


TEST_CASE("DeclarationParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens, unordered_map<string, DesignEntity> expected) {
        // given
        DeclarationParser parser = DeclarationParser(tokens);

        // when
        unordered_map<string, DesignEntity> actual = parser.parse();

        // then
        REQUIRE(actual == expected);
    };


    SECTION("One declaration") {
        testParseNoError(list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER),
                PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            unordered_map<string, DesignEntity>{ {"v1", DesignEntity::VARIABLE}});
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
            unordered_map<string, DesignEntity>{ {"v1", DesignEntity::VARIABLE},
            { "s1", DesignEntity::STMT }});
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
            unordered_map<string, DesignEntity>{ {"v1", DesignEntity::VARIABLE},
            { "s1", DesignEntity::STMT }, { "s2", DesignEntity::STMT }});
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
        testParseWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)});
    }

    SECTION("Missing comma") {
        testParseWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken("v2", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)});
    }

    SECTION("Repeated synonym name - same design entity") {
        testParseWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)});
    }

    SECTION("Repeated synonym name - different design entities") {
        testParseWithError(list<PQLToken>{PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("stmt", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER)});
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
        testGetRemainingTokens(list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(";", PQLTokenType::DELIMITER),
                PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            list<PQLToken>{PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)});
    }

    SECTION("Multiple declarations; such that and pattern included") {
        testGetRemainingTokens(list<PQLToken>{
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
                PQLToken(")", PQLTokenType::DELIMITER)},
            list<PQLToken>{PQLToken("Select", PQLTokenType::NAME), 
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken("such", PQLTokenType::NAME),
                PQLToken("that", PQLTokenType::NAME),
                PQLToken("Modifies", PQLTokenType::NAME),
                PQLToken("(", PQLTokenType::DELIMITER),
                PQLToken("1", PQLTokenType::DELIMITER),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("v1", PQLTokenType::DELIMITER),
                PQLToken(")", PQLTokenType::DELIMITER)});
    }
}