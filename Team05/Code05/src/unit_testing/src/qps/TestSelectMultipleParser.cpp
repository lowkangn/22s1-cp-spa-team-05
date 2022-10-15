#include "catch.hpp"
#include <qps/query_parser/parsers/SelectMultipleParser.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectMultipleParser: test parseNoError") {
	auto testParseNoError = [](list<PQLToken> tokens,
							   unordered_map<string, ArgumentType> declarations,
							   SelectClause expected) {
		// given
		SelectMultipleParser parser = SelectMultipleParser(tokens, declarations);

		// when
		shared_ptr<SelectClause> actualPtr = parser.parse();

		// then
		REQUIRE(expected.equals(actualPtr));
	};

	list<PQLToken> tokens = list<PQLToken>{
			PQLToken::createDelimiterToken("<"),
			PQLToken::createNameToken("v1"),
			PQLToken::createDelimiterToken(","),
			PQLToken::createNameToken("v2"),
			PQLToken::createDelimiterToken(">")
	};

	unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
			{"v1", ArgumentType::VARIABLE},
			{"v2", ArgumentType::VARIABLE}
	};

	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v1");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v2");
	SelectClause expected = SelectClause::createSynonymSelectClause({firstVarArg, secondVarArg});

	testParseNoError(tokens, declarations, expected);

	SECTION("Duplicate synonyms") {
		list<PQLToken> tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(">")
		};

		unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
				{"v1", ArgumentType::VARIABLE},
				{"v2", ArgumentType::VARIABLE}
		};

		expected = SelectClause::createSynonymSelectClause({firstVarArg, firstVarArg});
		testParseNoError(tokens, declarations, expected);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v2"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(">")
		};

		expected = SelectClause::createSynonymSelectClause({firstVarArg, secondVarArg, firstVarArg});
		testParseNoError(tokens, declarations, expected);
	}

	SECTION("Synonyms with attributes") {
		list<PQLToken> tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("r1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("varName"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("r1"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("c1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("procName"),
				PQLToken::createDelimiterToken(">")
		};

		unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
				{"r1", ArgumentType::READ},
				{"c1", ArgumentType::CALL},
		};

		ClauseArgument firstReadArg = ClauseArgument::createReadArg("r1");
		ClauseArgument firstReadArgVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(firstReadArg);
		ClauseArgument firstCallArg = ClauseArgument::createCallArg("c1");
		ClauseArgument firstCallArgProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(firstCallArg);

		expected = SelectClause::createSynonymSelectClause({ firstReadArg, firstReadArgVarNameAttributeArg,
			firstReadArg, firstCallArg, firstCallArgProcNameAttributeArg});
		testParseNoError(tokens, declarations, expected);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("varName"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("r1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("stmt"),
				PQLToken::createDelimiterToken("#"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("varName"),
				PQLToken::createDelimiterToken(">")
		};

		declarations = unordered_map<string, ArgumentType>{
				{"r1", ArgumentType::READ},
				{"v1", ArgumentType::VARIABLE},
		};
		
		ClauseArgument firstReadArgStmtNumArg = ClauseArgument::createStmtNumAttributeArg(firstReadArg);
		ClauseArgument firstVarArgVarNameArg = ClauseArgument::createVarNameAttributeArg(firstVarArg);

		expected = SelectClause::createSynonymSelectClause({ firstVarArg, firstVarArgVarNameArg,
			firstReadArg, firstReadArgStmtNumArg, firstVarArg, firstVarArgVarNameArg});
		testParseNoError(tokens, declarations, expected);
	}
}

TEST_CASE("SelectMultipleParser: test parseWithSyntaxError") {
	auto testParseWithSyntaxError = [](list<PQLToken> tokens,
								 unordered_map<string, ArgumentType> declarations) {
		// given
		SelectMultipleParser parser = SelectMultipleParser(tokens, declarations);

		// when & then
		REQUIRE_THROWS_AS(parser.parse(), PQLSyntaxError);
	};

	SECTION("Missing delimiters/synonyms") {
		list<PQLToken> tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v2")
		};

		unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
				{"v1", ArgumentType::VARIABLE},
				{"v2", ArgumentType::VARIABLE}
		};

		testParseWithSyntaxError(tokens, declarations);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createNameToken("v2"),
				PQLToken::createDelimiterToken(">")
		};

		testParseWithSyntaxError(tokens, declarations);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken(">")
		};

		testParseWithSyntaxError(tokens, declarations);
	}

	SECTION("Missing / unkown attribute") {
		list<PQLToken> tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v2"),
				PQLToken::createDelimiterToken(">")
		};

		unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
				{"v1", ArgumentType::VARIABLE},
				{"v2", ArgumentType::VARIABLE}
		};

		testParseWithSyntaxError(tokens, declarations);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("VARNAME"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v2"),
				PQLToken::createDelimiterToken(">")
		};

		testParseWithSyntaxError(tokens, declarations);
	}

	SECTION("Selecting non-synonyms") {
		list<PQLToken> tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createNameToken("v2"),
				PQLToken::createDelimiterToken("\""),
				PQLToken::createDelimiterToken(">")
		};

		unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
				{"v1", ArgumentType::VARIABLE},
				{"v2", ArgumentType::VARIABLE}
		};

		testParseWithSyntaxError(tokens, declarations);
	}
}

TEST_CASE("SelectMultipleParser: test parseWithSemanticError") {
	auto testParseWithSemanticError = [](list<PQLToken> tokens,
								 unordered_map<string, ArgumentType> declarations) {
		// given
		SelectMultipleParser parser = SelectMultipleParser(tokens, declarations);

		// when
		parser.parse();

		// then
		REQUIRE(!parser.isSemanticallyValid());
	};

	SECTION("Undeclared / misspelled synonym") {
		list<PQLToken> tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("v1"),
				PQLToken::createDelimiterToken(">")
		};

		unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
				{"v2", ArgumentType::VARIABLE}
		};

		testParseWithSemanticError(tokens, declarations);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("abcdefgLongName"),
				PQLToken::createDelimiterToken(">")
		};

		declarations = unordered_map<string, ArgumentType>{
				{"v1", ArgumentType::VARIABLE},
				{"v2", ArgumentType::CONSTANT}
		};

		testParseWithSemanticError(tokens, declarations);
	}

	SECTION("Mismatched attribute") {
		unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
				{"v1", ArgumentType::VARIABLE},
				{"i1", ArgumentType::IF},
				{"const1", ArgumentType::CONSTANT}
		};

		list<PQLToken> tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("const1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("value"),
				PQLToken::createDelimiterToken(","),
				PQLToken::createNameToken("v1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("procName"),
				PQLToken::createDelimiterToken(">")
		}; 
		testParseWithSemanticError(tokens, declarations);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("i1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("varName"),
				PQLToken::createDelimiterToken(">")
		};
		testParseWithSemanticError(tokens, declarations);

		tokens = list<PQLToken>{
				PQLToken::createDelimiterToken("<"),
				PQLToken::createNameToken("const1"),
				PQLToken::createOperatorToken("."),
				PQLToken::createNameToken("stmt"),
				PQLToken::createDelimiterToken("#"),
				PQLToken::createDelimiterToken(">")
		};
		testParseWithSemanticError(tokens, declarations);
	}
}
