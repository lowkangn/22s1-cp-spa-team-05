#include "catch.hpp"
#include <sp/lexer/Lexer.h>
#include <sp/lexer/Lexer.cpp>




// =============== UNIT TESTS ====================

TEST_CASE("Lexer: test charIsAlphabetical") {
    
        // ----- given -----
        Lexer lexer = Lexer();
        char alphabetical = 'a';


        // ----- when -----
        bool isAlphabetical = lexer.charIsAlphabetical(alphabetical);


        // ----- then -----
        REQUIRE(isAlphabetical);

}

TEST_CASE("Lexer: test charIsDigit") {



}

TEST_CASE("Lexer: test isDelimiter") {}

TEST_CASE("Lexer: test isOperator") {}

TEST_CASE("Lexer: test isWhiteSpace") {}

TEST_CASE("Lexer: test traverseStreamUntilNoWhiteSpace") {}

TEST_CASE("Lexer: test traverseStreamUntilNoNewLines") {}

// =============== INTEGRATION TESTS ====================

TEST_CASE("Lexer: test createNameTokenFromTraversingStream") {}

TEST_CASE("Lexer: test createIntegerTokenFromTraversingStream") {}

TEST_CASE("Lexer: test createDelimiterTokenFromTraversingStream") {}

TEST_CASE("Lexer: test createOperatorTokenFromTraversingStream") {}
