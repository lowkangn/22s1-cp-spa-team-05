#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query_parser/parsers/SelectParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        SelectClause expected) {
            // given
            SelectParser parser = SelectParser(tokens, declarations);

            // when
            shared_ptr<SelectClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };

    list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE}
    };

    SelectClause expected = SelectClause(ClauseArgument::createVariableArg("v1"));

    testParseNoError(tokens, declarations, expected);

    tokens = list<PQLToken>{
        PQLToken::createNameToken("Select"),
        PQLToken::createNameToken("abcdefgLongName")
    };

    declarations = unordered_map<string, ArgumentType>{{"abcdefgLongName", ArgumentType::CONSTANT}};
    expected = SelectClause(ClauseArgument::createConstantArg("abcdefgLongName"));

    testParseNoError(tokens, declarations, expected);
}

TEST_CASE("SelectParser: test parseWithSemanticError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            SelectParser parser = SelectParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLSemanticError);
    };

    SECTION("Undeclared / mispelled synonym") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1")
        };

        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("abcdefgLongName")
        };

        declarations = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"v2", ArgumentType::CONSTANT}
        };

        testParseWithError(tokens, declarations);
    }
    
    
}

TEST_CASE("SelectParser: test parseWithSyntaxError") {
    
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            SelectParser parser = SelectParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
    };

    SECTION("Selecting non-synonyms") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("Select"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createNameToken("main"),
            PQLToken::createDelimiterToken("\"")
        };

        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("Select"),
            PQLToken::createIntegerToken("1")
        };

        declarations = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"v2", ArgumentType::CONSTANT}
        };

        testParseWithError(tokens, declarations);
    }
}