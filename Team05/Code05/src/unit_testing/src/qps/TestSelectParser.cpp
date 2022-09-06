#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/SelectClause.cpp>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query_parser/parsers/ClauseParser.cpp>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/SelectParser.cpp>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        SelectClause expected) {
            // given
            SelectParser parser = SelectParser(tokens, declarations);

            // when
            shared_ptr<Clause>  actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };

    SelectClause expected = SelectClause(ClauseArgument("v1", ArgumentType::VARIABLE));
    testParseNoError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            unordered_map<string, ArgumentType>{ {"v1", ArgumentType::VARIABLE}},
            expected);

    expected = SelectClause(ClauseArgument("abcdefgLongName", ArgumentType::CONSTANT));
    testParseNoError(list<PQLToken>{
        PQLToken("Select", PQLTokenType::NAME),
            PQLToken("abcdefgLongName", PQLTokenType::NAME)},
        unordered_map<string, ArgumentType>{ {"abcdefgLongName", ArgumentType::CONSTANT}},
            expected);
}

TEST_CASE("SelectParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
            SelectParser parser = SelectParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };

    SECTION("Undeclared / mispelled synonym") {
        testParseWithError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            unordered_map<string, ArgumentType>{ {"v2", ArgumentType::VARIABLE}});
        testParseWithError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("abcdefgLongName", PQLTokenType::NAME)},
            unordered_map<string, ArgumentType>{
                {"v1", ArgumentType::VARIABLE}, {"v2", ArgumentType::CONSTANT}});
    }
    
    SECTION("Selecting non-synonyms") {
        testParseWithError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("\"", PQLTokenType::DELIMITER),
                PQLToken("main", PQLTokenType::NAME),
                PQLToken("\"", PQLTokenType::DELIMITER)},
            unordered_map<string, ArgumentType>{ {"v2", ArgumentType::VARIABLE}});

        testParseWithError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("1", PQLTokenType::INTEGER)},
            unordered_map<string, ArgumentType>{
                {"v1", ArgumentType::VARIABLE}, { "v2", ArgumentType::CONSTANT }});
    }
}
