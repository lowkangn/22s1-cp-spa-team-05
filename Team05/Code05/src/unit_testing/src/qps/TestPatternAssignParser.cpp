#include "catch.hpp"
#include <qps/query_parser/parsers/PatternParser.h>
#include <qps/query_parser/parsers/PatternAssignParser.h>
#include <iostream>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("PatternAssignParser: test parsePatternAssignNoError") {
	auto testParseNoError = [](list<PQLToken> tokens,
							   unordered_map<string, ArgumentType> declarations,
							   PatternAssignClause expected) {
		// given
		PatternAssignParser parser = PatternAssignParser(tokens, declarations);

		// when
		shared_ptr<PatternClause> actualPtr = parser.parse();

		// then
		REQUIRE(expected.equals(actualPtr));
	};

	SECTION("LHS variable, RHS wildcard") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE}
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createVariableArg("v"),
				ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS variable, RHS pattern string") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createOperatorToken("*"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("+"),
				PQLToken::createNameToken("y"),
				PQLToken::createOperatorToken("/"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("-"),
				PQLToken::createIntegerToken("3"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE}
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createVariableArg("v"),
				ClauseArgument::createPatternStringArg("x 2 * y 2 / + 3 -"));

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS variable, RHS string with wildcards") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createOperatorToken("*"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("+"),
				PQLToken::createNameToken("y"),
				PQLToken::createOperatorToken("/"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("-"),
				PQLToken::createIntegerToken("3"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE}
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createVariableArg("v"),
				ClauseArgument::createPatternStringWithWildcardsArg("_x 2 * y 2 / + 3 -_"));

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS string literal, RHS wildcard") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN}
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createStringLiteralArg("x"),
				ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS string literal, RHS string literal") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createOperatorToken("*"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("+"),
				PQLToken::createNameToken("y"),
				PQLToken::createOperatorToken("/"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("-"),
				PQLToken::createIntegerToken("3"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createStringLiteralArg("x"),
				ClauseArgument::createPatternStringArg("x 2 * y 2 / + 3 -"));

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS string literal, RHS string with wildcards") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createOperatorToken("*"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("+"),
				PQLToken::createNameToken("y"),
				PQLToken::createOperatorToken("/"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("-"),
				PQLToken::createIntegerToken("3"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createStringLiteralArg("x"),
				ClauseArgument::createPatternStringWithWildcardsArg("_x 2 * y 2 / + 3 -_"));

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS wildcard, RHS wildcard") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createWildcardArg(),
				ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS wildcard, RHS string literal") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createOperatorToken("*"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("+"),
				PQLToken::createNameToken("y"),
				PQLToken::createOperatorToken("/"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("-"),
				PQLToken::createIntegerToken("3"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createWildcardArg(),
				ClauseArgument::createPatternStringArg("x 2 * y 2 / + 3 -"));

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS wildcard, RHS string with wildcards") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createOperatorToken("*"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("+"),
				PQLToken::createNameToken("y"),
				PQLToken::createOperatorToken("/"),
				PQLToken::createIntegerToken("2"),
				PQLToken::createOperatorToken("-"),
				PQLToken::createIntegerToken("3"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
		};

		PatternAssignClause expected = PatternAssignClause(
				ClauseArgument::createAssignArg("a"),
				ClauseArgument::createWildcardArg(),
				ClauseArgument::createPatternStringWithWildcardsArg("_x 2 * y 2 / + 3 -_"));

		testParseNoError(tokensList, declarationsMap, expected);
	}

}

TEST_CASE("PatternAssignParser: test parseWithError") {
	auto testParseWithError = [](list<PQLToken> tokens,
								 unordered_map<string, ArgumentType> declarations) {
		// given
		PatternAssignParser parser = PatternAssignParser(tokens, declarations);

		// then
		REQUIRE_THROWS_AS(parser.parse(), PQLError);
	};

	SECTION("Invalid second argument") {

		// second arg not entRef or wildcard

		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("s"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"s", ArgumentType::STMT}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("r"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"r", ArgumentType::READ}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("p"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"p", ArgumentType::PRINT}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("a1"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"a1", ArgumentType::ASSIGN}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("c"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"c", ArgumentType::CALL}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"w", ArgumentType::WHILE}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"i", ArgumentType::IF}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("p"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"p", ArgumentType::PROCEDURE}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("c"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"c", ArgumentType::CONSTANT}
		};

		testParseWithError(tokensList, declarationsMap);
	}

	SECTION("Invalid third argument") {

		// Third arg is not wildcard/string

		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("s"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"s", ArgumentType::STMT}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("r"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"r", ArgumentType::READ}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("p"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"p", ArgumentType::PRINT}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("a1"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"a1", ArgumentType::ASSIGN}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("c"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"c", ArgumentType::CALL}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"w", ArgumentType::WHILE}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"i", ArgumentType::IF}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("p"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"p", ArgumentType::PROCEDURE}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"v1", ArgumentType::VARIABLE}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("a"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("c"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"a", ArgumentType::ASSIGN},
				{"v", ArgumentType::VARIABLE},
				{"c", ArgumentType::CONSTANT}
		};

		testParseWithError(tokensList, declarationsMap);
	}
}
