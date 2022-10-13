#include "catch.hpp"
#include <qps/query_parser/parsers/PatternParser.h>
#include <iostream>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("PatternParser: test parsePatternAssignNoError for Assign Patterns") {
	auto testParseNoError = [](list<PQLToken> tokens,
							   unordered_map<string, ArgumentType> declarations,
							   PatternAssignClause expected) {
		// given
		PatternParser parser = PatternParser(tokens, declarations);

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

TEST_CASE("PatternParser: test parsePatternWhileNoError for While Clause") {
	auto testParseNoError = [](list<PQLToken> tokens,
		unordered_map<string, ArgumentType> declarations,
		PatternWhileClause expected) {
			// given
			PatternParser parser = PatternParser(tokens, declarations);

			// when
			shared_ptr<PatternClause> actualPtr = parser.parse();

			// then
			REQUIRE(expected.equals(actualPtr));
	};

	SECTION("LHS variable, RHS wildcard for while patterns") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"w", ArgumentType::WHILE},
				{"v", ArgumentType::VARIABLE}
		};

		PatternWhileClause expected = PatternWhileClause(
			ClauseArgument::createWhileArg("w"),
			ClauseArgument::createVariableArg("v"),
			ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS variable, RHS wildcard for while patterns") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"w", ArgumentType::WHILE},
				{"v", ArgumentType::VARIABLE}
		};

		PatternWhileClause expected = PatternWhileClause(
			ClauseArgument::createWhileArg("w"),
			ClauseArgument::createVariableArg("v"),
			ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS string literal, RHS wildcard") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"w", ArgumentType::WHILE}
		};

		PatternWhileClause expected = PatternWhileClause(
			ClauseArgument::createWhileArg("w"),
			ClauseArgument::createStringLiteralArg("x"),
			ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS wildcard, RHS wildcard") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"w", ArgumentType::WHILE},
		};

		PatternWhileClause expected = PatternWhileClause(
			ClauseArgument::createWhileArg("w"),
			ClauseArgument::createWildcardArg(),
			ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}
}

TEST_CASE("PatternParser: test parsePatternIfNoError for If Clause") {
	auto testParseNoError = [](list<PQLToken> tokens,
		unordered_map<string, ArgumentType> declarations,
		PatternIfClause expected) {
			// given
			PatternParser parser = PatternParser(tokens, declarations);

			// when
			shared_ptr<PatternClause> actualPtr = parser.parse();

			// then
			REQUIRE(expected.equals(actualPtr));
	};

	SECTION("LHS variable") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"i", ArgumentType::IF},
				{"v", ArgumentType::VARIABLE}
		};

		PatternIfClause expected = PatternIfClause(
			ClauseArgument::createIfArg("i"),
			ClauseArgument::createVariableArg("v"),
			ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("LHS string literal") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("x"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"i", ArgumentType::IF}
		};

		PatternIfClause expected = PatternIfClause(
			ClauseArgument::createIfArg("i"),
			ClauseArgument::createStringLiteralArg("x"),
			ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}

	SECTION("All wildcard") {
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"i", ArgumentType::IF},
		};

		PatternIfClause expected = PatternIfClause(
			ClauseArgument::createIfArg("i"),
			ClauseArgument::createWildcardArg(),
			ClauseArgument::createWildcardArg());

		testParseNoError(tokensList, declarationsMap, expected);
	}
}

TEST_CASE("PatternParser: test parseWithSemanticError") {
	auto testParseWithError = [](list<PQLToken> tokens,
		unordered_map<string, ArgumentType> declarations) {
			// given
			PatternParser parser = PatternParser(tokens, declarations);

			// when
			parser.parse();

			// then
			REQUIRE(!parser.isSemanticallyValid());
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

		// second arg of if/while pattern not entRef or wildcard

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("s"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"w", ArgumentType::WHILE},
				{"s", ArgumentType::STMT}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("s"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"i", ArgumentType::IF},
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

	SECTION("Pattern if/while invalid third argument") {

		// third arg is a variable synonym
		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("w"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("t"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"w", ArgumentType::WHILE},
				{"t", ArgumentType::VARIABLE}
		};

		testParseWithError(tokensList, declarationsMap);

		tokensList = list<PQLToken>{
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("t"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(")")
		};

		declarationsMap = unordered_map<string, ArgumentType>{
				{"i", ArgumentType::IF},
				{"t", ArgumentType::VARIABLE}
		};

		testParseWithError(tokensList, declarationsMap);
	}
}

TEST_CASE("PatternParser: test parseWithSyntaxError") {
	auto testParseWithError = [](list<PQLToken> tokens,
		unordered_map<string, ArgumentType> declarations) {
			// given
			PatternParser parser = PatternParser(tokens, declarations);

			// then
			REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
	};

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

	SECTION("Pattern if invalid fourth argument") {

		list<PQLToken> tokensList = list<PQLToken>{
				PQLToken::createNameToken("i"),
				PQLToken::createDelimiterToken("("),
				PQLToken::createNameToken("v"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createNameToken("t"),
				PQLToken::createDelimiterToken("_"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(")")
		};

		unordered_map<string, ArgumentType> declarationsMap = unordered_map<string, ArgumentType>{
				{"i", ArgumentType::IF},
				{"t", ArgumentType::VARIABLE}
		};

		testParseWithError(tokensList, declarationsMap);
	}
}
