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
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE}
    };

    SelectClause expected = SelectClause(ClauseArgument("v1", ArgumentType::VARIABLE));

    testParseNoError(tokens, declarations, expected);

    tokens = list<PQLToken>{
        PQLToken("Select", PQLTokenType::NAME),
        PQLToken("abcdefgLongName", PQLTokenType::NAME)
    };

    declarations = unordered_map<string, ArgumentType>{{"abcdefgLongName", ArgumentType::CONSTANT}};
    expected = SelectClause(ClauseArgument("abcdefgLongName", ArgumentType::CONSTANT));

    testParseNoError(tokens, declarations, expected);
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
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME)
        };

        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("abcdefgLongName", PQLTokenType::NAME)
        };

        declarations = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"v2", ArgumentType::CONSTANT}
        };

        testParseWithError(tokens, declarations);
    }
    
    SECTION("Selecting non-synonyms") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken("main", PQLTokenType::NAME),
            PQLToken("\"", PQLTokenType::DELIMITER)
        };

        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("1", PQLTokenType::INTEGER)
        };

        declarations = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"v2", ArgumentType::CONSTANT}
        };

        testParseWithError(tokens, declarations);
    }
}
