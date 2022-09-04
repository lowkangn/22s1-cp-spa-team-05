#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query_parser/parsers/ClauseParser.cpp>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/SelectParser.cpp>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, DesignEntity> declarations,
        SelectClause expected) {
            // given
            SelectParser parser = SelectParser(tokens, declarations);

            // when
            SelectClause actual = *parser.parse();
            
            // then
            REQUIRE(actual == expected);
    };

    SelectClause expected = SelectClause(ClauseArgument("v1", ArgumentType::SYNONYM));
    testParseNoError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            unordered_map<string, DesignEntity>{ {"v1", DesignEntity::VARIABLE}},
            expected);

    expected = SelectClause(ClauseArgument("abcdefgLongName", ArgumentType::SYNONYM));
    testParseNoError(list<PQLToken>{
        PQLToken("Select", PQLTokenType::NAME),
            PQLToken("abcdefgLongName", PQLTokenType::NAME)},
        unordered_map<string, DesignEntity>{ {"abcdefgLongName", DesignEntity::CONSTANT}},
            expected);
}

TEST_CASE("SelectParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, DesignEntity> declarations) {
            // given
            SelectParser parser = SelectParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };

    SECTION("Undeclared / mispelled synonym") {
        testParseWithError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("v1", PQLTokenType::NAME)},
            unordered_map<string, DesignEntity>{ {"v2", DesignEntity::VARIABLE}});
        testParseWithError(list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
                PQLToken("abcdefgLongName", PQLTokenType::NAME)},
            unordered_map<string, DesignEntity>{ 
                {"v1", DesignEntity::VARIABLE}, {"v2", DesignEntity::CONSTANT}});
    }
    
}
