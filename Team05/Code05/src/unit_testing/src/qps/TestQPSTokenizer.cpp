#include "catch.hpp"
#include <sstream>
#include <string>
#include <list>
#include <qps/query_tokenizer/QPSTokenizer.h>
#include <iostream>
using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("QPSTokenizer: test isAlphabetical") {

    auto testIsAlphabetical = [](char alphabetical, bool expectedResult) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();

        // ----- when -----
        bool isCharAlphabetical = tokenizer.isAlphabetical(alphabetical);

        // ----- then -----
        REQUIRE(isCharAlphabetical == expectedResult);
    };

    // run tests
    testIsAlphabetical('a', true);
    testIsAlphabetical('b', true);
    testIsAlphabetical('1', false);
    testIsAlphabetical('2', false);
    testIsAlphabetical(':', false);
    testIsAlphabetical(' ', false);

}

TEST_CASE("QPSTokenizer: test isDigit") {

    auto testIsDigit = [](char digit, bool expectedResult) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();

        // ----- when -----
        bool isCharDigit = tokenizer.isDigit(digit);

        // ----- then -----
        REQUIRE(isCharDigit == expectedResult);
    };

    // run tests
    testIsDigit('a', false);
    testIsDigit('b', false);
    testIsDigit('1', true);
    testIsDigit('2', true);
    testIsDigit(':', false);
    testIsDigit(' ', false);

}

TEST_CASE("QPSTokenizer: test isDelimiter") {

    auto testIsDelimiter = [](char del, bool expectedResult) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();

        // ----- when -----
        bool isCharDelimiter = tokenizer.isDelimiter(del);

        // ----- then -----
        REQUIRE(isCharDelimiter == expectedResult);
    };

    // run tests
    testIsDelimiter('a', false);
    testIsDelimiter('1', false);
    testIsDelimiter(':', false);
    testIsDelimiter(';', true);
    testIsDelimiter('.', true);
    testIsDelimiter(',', true);
    testIsDelimiter('{', false);
    testIsDelimiter('}', false);
    testIsDelimiter('(', true);
    testIsDelimiter(')', true);
    testIsDelimiter('+', false);
    testIsDelimiter('-', false);
    testIsDelimiter('*', false);
    testIsDelimiter('/', false);
    testIsDelimiter('%', false);
    testIsDelimiter('#', true);
    testIsDelimiter('|', false);
    testIsDelimiter('&', false);
    testIsDelimiter('!', false);
    testIsDelimiter('_', true);
    testIsDelimiter('\"', true);
    testIsDelimiter('\'', false);
    testIsDelimiter('\n', false);

}

TEST_CASE("QPSTokenizer: test isOperator") {

    auto testIsOperator = [](char op, bool expectedResult) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();

        // ----- when -----
        bool isCharOperator = tokenizer.isOperator(op);

        // ----- then -----
        REQUIRE(isCharOperator == expectedResult);
    };

    // run tests
    testIsOperator('a', false);
    testIsOperator('1', false);
    testIsOperator(':', false);
    testIsOperator('.', false);
    testIsOperator(',', false);
    testIsOperator('{', false);
    testIsOperator('}', false);
    testIsOperator('(', false);
    testIsOperator(')', false);
    testIsOperator(';', false);
    testIsOperator('+', true);
    testIsOperator('-', true);
    testIsOperator('*', true);
    testIsOperator('/', true);
    testIsOperator('%', true);
    testIsOperator('#', false);
    testIsOperator('|', false);
    testIsOperator('&', false);
    testIsOperator('!', false);
    testIsOperator('_', false);
    testIsOperator('\"', false);
    testIsOperator('\'', false);
    testIsOperator('\n', false);

}

TEST_CASE("PQSTokenizer: test isWhitespaceOrNewline") {

    auto testIsWhitespaceOrNewline = [](char ws, bool expectedResult) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();

        // ----- when -----
        bool isCharWhitespaceOrNewline = tokenizer.isWhitespaceOrNewline(ws);

        // ----- then -----
        REQUIRE(isCharWhitespaceOrNewline == expectedResult);
    };

    // run tests
    testIsWhitespaceOrNewline('a', false);
    testIsWhitespaceOrNewline('1', false);
    testIsWhitespaceOrNewline(':', false);
    testIsWhitespaceOrNewline(';', false);
    testIsWhitespaceOrNewline('.', false);
    testIsWhitespaceOrNewline(',', false);
    testIsWhitespaceOrNewline(' ', true);
    testIsWhitespaceOrNewline('_', false);
    testIsWhitespaceOrNewline('\"', false);
    testIsWhitespaceOrNewline('\'', false);
    testIsWhitespaceOrNewline('\n', true);

}

TEST_CASE("QPSTokenizer: test skipWhitespaceOrNewlineInStream") {

    auto test = [](string s, char expectedEndChar) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        tokenizer.skipWhitespaceOrNewlineInStream(stream);

        // ----- then -----
        REQUIRE(char(stream.peek()) == expectedEndChar);
    };

    SECTION("Only space") {
        test("   ", EOF);
    }

    SECTION("Only newline") {
        test("\n", EOF);
    }

    SECTION("Newline and whitespace") {
        test("      \n       \n     ", EOF);
    }

    SECTION("Newline and whitespace with other characters") {
        test(" something   ", 's');
        test(" \n     something_else   \n  ", 's');
        test("\n   ...", '.');
    }

    SECTION("Does nothing") {
        test("", EOF);
    }

}

TEST_CASE("QPSTokenizer: test extractNameFromStream") {
    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        PQLToken t = tokenizer.extractNameFromStream(stream);

        // ----- then -----
        REQUIRE(t.getTokenString() == expectedStringOfToken);
        REQUIRE(t.getTokenType() == PQLTokenType::NAME);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Some valid name") {
        test("myFavouriteVariable", "myFavouriteVariable", EOF);
    }

    SECTION("Some characters after not used") {
        test("assign a;\nSelect a;", "assign", ' ');
        test("a1;\nSelect a1;", "a1", ';');
        test("a11b22;\nSelect a11b22;", "a11b22", ';');
    }

}

TEST_CASE("QPSTokenizer: test extractIntegerFromStream correct extracts") {
    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        PQLToken t = tokenizer.extractIntegerFromStream(stream);

        // ----- then -----
        REQUIRE(t.getTokenString() == expectedStringOfToken);
        REQUIRE(t.getTokenType() == PQLTokenType::INTEGER);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Some valid integer") {
        test("1234", "1234", EOF);
    }

    SECTION("Some characters after not used") {
        test("1234 ", "1234", ' ');
        test("5678+999999", "5678", '+');
        test("1000-7", "1000", '-');
        test("123*123", "123", '*');
        test("7/11", "7", '/');
        test("33)", "33", ')');
        test("34\"", "34", '\"');
    }

}

TEST_CASE("QPSTokenizer: test extractIntegerFromStream correct throws") {
    auto test = [](string s) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when & then -----
        REQUIRE_THROWS(tokenizer.extractIntegerFromStream(stream));
    };

    SECTION("Integer followed by alphabetical character") {
        test("1234Z");
    }

}

TEST_CASE("QPSTokenizer: test extractDelimiterFromStream") {

    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        PQLToken t = tokenizer.extractDelimiterFromStream(stream);

        // ----- then -----
        REQUIRE(t.getTokenString() == expectedStringOfToken);
        REQUIRE(t.getTokenType() == PQLTokenType::DELIMITER);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Valid delimiter only") {
        test(",", ",", EOF);
        test(";", ";", EOF);
        test("(", "(", EOF);
        test(")", ")", EOF);
        test("_", "_", EOF);
        test("\"", "\"", EOF);
    }

    SECTION("Valid delimiter followed by characters") {
        test(",a1;", ",", 'a');
        test("; Select v;", ";", ' ');
    }
}

TEST_CASE("QPSTokenizer: test extractOperatorFromStream") {
    auto test = [](string s, string expectedStringOfToken, char expectedNextChar) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        PQLToken t = tokenizer.extractOperatorFromStream(stream);

        // ----- then -----
        REQUIRE(t.getTokenString() == expectedStringOfToken);
        REQUIRE(t.getTokenType() == PQLTokenType::OPERATOR);
        REQUIRE(char(stream.peek()) == expectedNextChar);
    };

    SECTION("Valid operators only") {
        test("+", "+", EOF);
        test("-", "-", EOF);
        test("*", "*", EOF);
        test("/", "/", EOF);
        test("%", "%", EOF);
    }

    SECTION("Valid operators followed by characters") {
        test("+3) * 3", "+", '3');
        test("-4/2", "-", '4');
        test("* 2 * 3 * 4", "*", ' ');
        test("/ 3 * 3 / 3 * 3", "/", ' ');
        test("% 3) + x", "%", ' ');
    }

}

// =============== INTEGRATION TESTS ====================

TEST_CASE("QPSTokenizer: test tokenize works correctly") {

    auto test = [](string s, list<PQLToken> expectedTokens) {
        // ----- given -----
        QPSTokenizer tokenizer = QPSTokenizer();
        stringstream ss(s);
        istream& stream = ss;

        // ----- when -----
        list<PQLToken> tokens = tokenizer.tokenize(stream);

        // ---- then -----
        PQLToken curr = tokens.front();
        PQLToken currExpected = expectedTokens.front();
        while (!tokens.empty() && !expectedTokens.empty()) {

            curr = tokens.front();
            currExpected = expectedTokens.front();

            REQUIRE(curr.equals(currExpected));

            tokens.pop_front();
            expectedTokens.pop_front();

        }

    };

    SECTION("short input") {
        string testString = "variable v1; Select v1";

        list<PQLToken> expectedTokensList = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
        };

        test(testString, expectedTokensList);
    }

    SECTION("long input with spaces") {
        string testString = "stmt c; variable \t\tt \t, v1; \n Select t such \n\t that Modifies(c, v1)";

        list<PQLToken> expectedTokensList = list<PQLToken>{
            PQLToken::createNameToken("stmt"),
            PQLToken::createNameToken("c"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("t"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("t"),
            PQLToken::createNameToken("such"),
            PQLToken::createNameToken("that"),
            PQLToken::createNameToken("Modifies"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("c"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(")")
        };

        test(testString, expectedTokensList);
    }

}
