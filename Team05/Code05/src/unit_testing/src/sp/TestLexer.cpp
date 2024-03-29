#include "catch.hpp"
#include <sstream>
#include <string>
#include <list>
#include <sp/lexer/Lexer.h>
#include <iostream>
using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("Lexer: test charIsAlphabetical") {
    
    auto testCharIsAlphebetical = [](char alphabetical, bool expectedResult) {
        // ----- given -----
        Lexer lexer = Lexer();

        // ----- when -----
        bool isAlphabetical = lexer.charIsAlphabetical(alphabetical);

        // ----- then -----
        REQUIRE(isAlphabetical == expectedResult);
    };

    // run tests
    testCharIsAlphebetical('a', true);
    testCharIsAlphebetical('b', true);
    testCharIsAlphebetical('1', false);
    testCharIsAlphebetical(':', false);

}

TEST_CASE("Lexer: test charIsDigit") {

    auto testCharIsDigit = [](char digit, bool expectedResult) {
        // ----- given -----
        Lexer lexer = Lexer();

        // ----- when -----
        bool isDigit = lexer.charIsDigit(digit);

        // ----- then -----
        REQUIRE(isDigit == expectedResult);
    };

    // run tests
    testCharIsDigit('a', false);
    testCharIsDigit('b', false);
    testCharIsDigit('1', true);
    testCharIsDigit(':', false);

}

TEST_CASE("Lexer: test icharIsDelimiter") {
    auto testCharIsDelimiter = [](char digit, bool expectedResult) {
        // ----- given -----
        Lexer lexer = Lexer();

        // ----- when -----
        bool isDelimiter = lexer.charIsDelimiter(digit);

        // ----- then -----
        REQUIRE(isDelimiter == expectedResult);
    };

    // run tests
    testCharIsDelimiter('a', false);
    testCharIsDelimiter('b', false);
    testCharIsDelimiter('1', false);
    testCharIsDelimiter(':', false);
    testCharIsDelimiter('.', false);
    testCharIsDelimiter(',', false);
    testCharIsDelimiter('{', true);
    testCharIsDelimiter('}', true);
    testCharIsDelimiter('(', true);
    testCharIsDelimiter(')', true);
    testCharIsDelimiter(';', true);

}

TEST_CASE("Lexer: test charIsOperator") {
    auto testCharIsOperator = [](char digit, bool expectedResult) {
        // ----- given -----
        Lexer lexer = Lexer();

        // ----- when -----
        bool isOperator = lexer.charIsOperator(digit);

        // ----- then -----
        REQUIRE(isOperator == expectedResult);
    };

    // run tests
    testCharIsOperator('a', false);
    testCharIsOperator('b', false);
    testCharIsOperator('1', false);
    testCharIsOperator(':', false);
    testCharIsOperator('.', false);
    testCharIsOperator(',', false);
    testCharIsOperator('{', false);
    testCharIsOperator('}', false);
    testCharIsOperator('(', false);
    testCharIsOperator(')', false);
    testCharIsOperator(';', false);
    testCharIsOperator('+', true);
    testCharIsOperator('-', true);
    testCharIsOperator('*', true);
    testCharIsOperator('/', true);
    testCharIsOperator('%', true);
    testCharIsOperator('#', false);
    testCharIsOperator('|', true);
    testCharIsOperator('&', true);
    testCharIsOperator('!', true);
}

TEST_CASE("Lexer: test charIsWhiteSpace") {

    auto testCharIsWhiteSpace = [](char digit, bool expectedResult) {
        // ----- given -----
        Lexer lexer = Lexer();

        // ----- when -----
        bool isWhiteSpace = lexer.charIsWhiteSpace(digit);

        // ----- then -----
        REQUIRE(isWhiteSpace == expectedResult);
    };

    // run tests
    testCharIsWhiteSpace('a', false);
    testCharIsWhiteSpace('b', false);
    testCharIsWhiteSpace('1', false);
    testCharIsWhiteSpace(':', false);
    testCharIsWhiteSpace('.', false);
    testCharIsWhiteSpace(',', false);
    testCharIsWhiteSpace(' ', true);

}

TEST_CASE("Lexer: test traverseStreamUntilNoWhiteSpace correctly consumes tokens") {
    auto test = [](string s, char expectedEndChar) {
        // ----- given -----
        Lexer lexer = Lexer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        lexer.traverseStreamUntilNoWhiteSpace(stream);

        // ----- then -----
        REQUIRE(char(stream.peek()) == expectedEndChar);
    };

    SECTION("Only space") {
        test("   ", EOF);
    }

    SECTION("Space in the middle") {
        test(" something   ", 's');
    }
    SECTION("Does nothing") {
        test("", EOF); // only backslash is ok
    }

}


TEST_CASE("Lexer: test createNameTokenFromTraversingStream") {
    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        Lexer lexer = Lexer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        Token t = lexer.createKeywordOrNameTokenFromTraversingStream(stream);

        // ----- then -----
        REQUIRE(t.getString() == expectedStringOfToken);
        REQUIRE(t.getType() == TokenType::NAME_OR_KEYWORD);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Some valid name") {
        test("someVariable123", "someVariable123", EOF);
    }

    SECTION("Some characters after not used") {
        test("somevariable ", "somevariable", ' ');
    }

    SECTION("Some valid name") {
        test("hello=", "hello", '=');
    }

}

TEST_CASE("Lexer: test createIntegerTokenFromTraversingStream") {
    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        Lexer lexer = Lexer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        Token t = lexer.createIntegerTokenFromTraversingStream(stream);

        // ----- then -----
        REQUIRE(t.getString() == expectedStringOfToken);
        REQUIRE(t.getType() == TokenType::INTEGER);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Some valid integer") {
        test("1234", "1234", EOF);
    }

    SECTION("Some characters after not used") {
        test("1234 ", "1234", ' ');
    }


}

TEST_CASE("Lexer: test createDelimiterTokenFromTraversingStream") {

    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        Lexer lexer = Lexer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        Token t = lexer.createDelimiterTokenFromTraversingStream(stream);

        // ----- then -----
        REQUIRE(t.getString() == expectedStringOfToken);
        REQUIRE(t.getType() == TokenType::DELIMITER);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Some valid delimiter") {
        test(";", ";", EOF);
        test("(", "(", EOF);
        test(")", ")", EOF);
        test("{", "{", EOF);
        test("}", "}", EOF);
    }

    SECTION("Some characters after not used") {
        test("; ", ";", ' ');
    }
}

TEST_CASE("Lexer: test createOperatorTokenFromTraversingStream") {
    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        Lexer lexer = Lexer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        Token t = lexer.createOperatorTokenFromTraversingStream(stream);

        // ----- then -----
        REQUIRE(t.getString() == expectedStringOfToken);
        REQUIRE(t.getType() == TokenType::OPERATOR);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Basic operators") {
        test("+ ", "+", ' ');
        test("- ", "-", ' ');
        test("* ", "*", ' ');
        test("/ ", "/", ' ');
    }

    SECTION("Comparative operators") {
        test("> ", ">", ' ');
        test("< ", "<", ' ');
        test("= ", "=", ' ');
        test(">= ", ">=", ' ');
        test("<= ", "<=", ' ');
        test("== ", "==", ' ');
        test("!= ", "!=", ' ');
    }

    SECTION("Logical operators") {
        test("&& ", "&&", ' ');
        test("|| ", "||", ' ');
    }

   
}

TEST_CASE("Lexer: test createOperatorTokenFromTraversingStream correctly throws") {
    auto test = [](string s) {
        // ----- given -----
        Lexer lexer = Lexer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when & then -----
        REQUIRE_THROWS(lexer.createOperatorTokenFromTraversingStream(stream));
    };

    SECTION("sole not ! operator") {
        test("!");
    }

    SECTION("Some wrong operator combinations") {
        test(">>");
        test("|&");
        test("&>");
        test("&&&");
    }

}

// =============== INTEGRATION TESTS ====================

TEST_CASE("Lexer: test tokenize works correctly") {

    auto test = [](string s, list<Token> expectedTokens) {
        // ----- given -----
        Lexer lexer = Lexer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        list<Token> tokens = lexer.tokenize(stream);

        // ---- then -----
        Token curr = tokens.front();
        Token currExpected = expectedTokens.front(); 
        while (!tokens.empty() && !expectedTokens.empty()) {

            curr = tokens.front();
            currExpected = expectedTokens.front();
            if (!(curr == currExpected)) {
                // cout << curr.getString() << " " << currExpected.getString() << endl; for debugging
            }
            REQUIRE(curr == currExpected);

            tokens.pop_front();
            expectedTokens.pop_front();

            
        }
        
    };

    SECTION("Single line") {
        test("a + b = c", list<Token>{
            Token::createNameOrKeywordToken("a"), 
                Token::createPlusToken(),
                Token::createNameOrKeywordToken("b"),
                Token::createEqualsToken(),
                Token::createNameOrKeywordToken("c")
        });
    }

    SECTION("Multiple lines") {
        test("a + b = c;\nd+e=f", list<Token>{
            Token::createNameOrKeywordToken("a"),
                Token::createPlusToken(),
                Token::createNameOrKeywordToken("b"),
                Token::createEqualsToken(),
                Token::createNameOrKeywordToken("c"),
                Token::createSemicolonToken(),
                Token::createNameOrKeywordToken("d"),
                Token::createPlusToken(),
                Token::createNameOrKeywordToken("e"),
                Token::createEqualsToken(),
                Token::createNameOrKeywordToken("f")
        });
    }

    SECTION("With nested while") {
        test("\nwhile (! ((1==0) && (1==0))) {\n    x = 1;\n}\n", list<Token>{
            Token::createNameOrKeywordToken("while"),
                Token::createOpenBracketToken(),
                Token::createNotToken(),
                Token::createOpenBracketToken(),
                Token::createOpenBracketToken(),
                Token::createIntegerToken("1"),
                Token::createEqualityToken(),
                Token::createIntegerToken("0"),
                Token::createCloseBracketToken(),

                Token::createAndToken(),
                Token::createOpenBracketToken(),
                Token::createIntegerToken("1"),
                Token::createEqualityToken(),
                Token::createIntegerToken("0"),
                Token::createCloseBracketToken(),
                Token::createCloseBracketToken(),
                Token::createCloseBracketToken(),
                Token::createOpenCurlyBracketToken(),
                Token::createNameOrKeywordToken("x"),
                Token::createEqualsToken(),
                Token::createIntegerToken("1"),
                Token::createSemicolonToken(),
                Token::createCloseCurlyBracketToken(),
        });
    }

    SECTION("With nested while, but another") {
        test("\nwhile (1>= 1%((0-1)) ) {\n    x = 1;\n}\n", list<Token>{
            Token::createNameOrKeywordToken("while"),
                Token::createOpenBracketToken(),
                Token::createIntegerToken("1"),
                Token::createGreaterThanEqualToken(),
                Token::createIntegerToken("1"),
                Token::createModulusToken(),
                Token::createOpenBracketToken(),
                Token::createOpenBracketToken(),
                Token::createIntegerToken("0"),
                Token::createMinusToken(),
                Token::createIntegerToken("1"),
                Token::createCloseBracketToken(),
                Token::createCloseBracketToken(),
                Token::createCloseBracketToken(),
                Token::createOpenCurlyBracketToken(),
                Token::createNameOrKeywordToken("x"),
                Token::createEqualsToken(),
                Token::createIntegerToken("1"),
                Token::createSemicolonToken(),
                Token::createCloseCurlyBracketToken(),
        });
    }

    SECTION("With nested while, but another") {
        test("\nwhile (1>= 1%((1)) )  {\n    x = 1;\n}\n", list<Token>{
            Token::createNameOrKeywordToken("while"),
                Token::createOpenBracketToken(),

                Token::createIntegerToken("1"),
                Token::createGreaterThanEqualToken(),
                Token::createIntegerToken("1"),
                Token::createModulusToken(),
                Token::createOpenBracketToken(),
                Token::createOpenBracketToken(),
                Token::createIntegerToken("1"),
                Token::createCloseBracketToken(),
                Token::createCloseBracketToken(),

                Token::createCloseBracketToken(),
                Token::createOpenCurlyBracketToken(),
                Token::createNameOrKeywordToken("x"),
                Token::createEqualsToken(),
                Token::createIntegerToken("1"),
                Token::createSemicolonToken(),
                Token::createCloseCurlyBracketToken(),
        });
    }

    SECTION("Empty while condition") {
        test("\nwhile ( )  {\n    x = 1;\n}\n", list<Token>{
            Token::createNameOrKeywordToken("while"),
                Token::createOpenBracketToken(),
                Token::createCloseBracketToken(),
                Token::createOpenCurlyBracketToken(),
                Token::createNameOrKeywordToken("x"),
                Token::createEqualsToken(),
                Token::createIntegerToken("1"),
                Token::createSemicolonToken(),
                Token::createCloseCurlyBracketToken(),
        });
    }
    


}