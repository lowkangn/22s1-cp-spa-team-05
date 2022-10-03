#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>
#include <qps/query_parser/parsers/ClauseParser.h>
#include <qps/query_parser/parsers/SelectSingleParser.h>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectSingleParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        SelectClause expected) {
            // given
			SelectSingleParser parser = SelectSingleParser(tokens, declarations);

            // when
            shared_ptr<SelectClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };

    list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("v1")
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE}
    };

	ClauseArgument varArg = ClauseArgument::createVariableArg("v1");
    SelectClause expected = SelectClause::createSynonymSelectClause({varArg});

    testParseNoError(tokens, declarations, expected);

    tokens = list<PQLToken>{
        PQLToken::createNameToken("abcdefgLongName")
    };

    declarations = unordered_map<string, ArgumentType>{{"abcdefgLongName", ArgumentType::CONSTANT}};
    ClauseArgument constArg = ClauseArgument::createConstantArg("abcdefgLongName");
	expected = SelectClause::createSynonymSelectClause({constArg});

    testParseNoError(tokens, declarations, expected);
}

TEST_CASE("SelectSingleParser: test parseWithSemanticError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
			SelectSingleParser parser = SelectSingleParser(tokens, declarations);

            // when
            parser.parse();

            // then
            REQUIRE(!parser.isSemanticallyValid());
    };

    SECTION("Undeclared / mispelled synonym") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("v1")
        };

        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("abcdefgLongName")
        };

        declarations = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"v2", ArgumentType::CONSTANT}
        };

        testParseWithError(tokens, declarations);
    }
    
    
}

// TODO: Shift this test to QueryParser: passing of illegal arguments after 'Select'
//TEST_CASE("SelectSingleParser: test parseWithSyntaxError") {
//
//    auto testParseWithError = [](list<PQLToken> tokens,
//        unordered_map<string, ArgumentType> declarations) {
//            // given
//			SelectSingleParser parser = SelectSingleParser(tokens, declarations);
//
//            // then
//            REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
//    };
//
//
//	SECTION("Selecting non-synonyms") {
//        list<PQLToken> tokens = list<PQLToken>{
//            PQLToken::createDelimiterToken("\""),
//            PQLToken::createNameToken("main"),
//            PQLToken::createDelimiterToken("\"")
//        };
//
//        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
//            {"v2", ArgumentType::VARIABLE}
//        };
//
//        testParseWithError(tokens, declarations);
//
//        tokens = list<PQLToken>{
//            PQLToken::createIntegerToken("1")
//        };
//
//        declarations = unordered_map<string, ArgumentType>{
//            {"v1", ArgumentType::VARIABLE},
//            {"v2", ArgumentType::CONSTANT}
//        };
//
//        testParseWithError(tokens, declarations);
//    }
//}