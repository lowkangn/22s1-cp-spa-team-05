#include "catch.hpp"

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>
#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/PrintSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/ExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/RelationalExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/ConditionalExpressionSimpleSyntaxRule.h>
#include <sp/parser/rules/OperatorSimpleSyntaxRule.h>
#include <sp/parser/rules/StatementListSimpleSyntaxRule.h>
#include <sp/parser/rules/WhileSimpleSyntaxRule.h>
#include <sp/parser/rules/IfSimpleSyntaxRule.h>

#include <sp/dataclasses/tokens/Token.h>
#include <list>
#include <memory>
using namespace std;

// =============== UNIT TEST FOR RULES ====================

TEST_CASE("Parser: test ::consumeTokens") {

	auto test = [](SimpleSyntaxRule& rule, list<Token> tokens, list<Token> expectedTokens) {

		// ----- when -----
		list<Token> remainingTokens = rule.consumeTokens(tokens);

		// ----- then -----
		bool sameSize = remainingTokens.size() == expectedTokens.size();
		REQUIRE(sameSize); // same size
		if (sameSize) {
			while (!remainingTokens.empty()) {
				Token t1 = remainingTokens.front();
				Token t2 = expectedTokens.front();
				REQUIRE(t1 == t2);
				remainingTokens.pop_front();
				expectedTokens.pop_front();
			}
		}
	};

	auto testThrowsException = [](SimpleSyntaxRule& rule, list<Token> tokens) {

		// ----- when & then -----
		REQUIRE_THROWS(rule.consumeTokens(tokens));
	};

	// -------------------- ConstantValueSimpleSyntaxRule --------------------
	SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, numeric") {
		list<Token> tokens = { Token::createIntegerToken("1") };
		list<Token> expectedTokens = {};
		test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, very large number") {
		list<Token> tokens = { Token::createIntegerToken("12345435") };
		list<Token> expectedTokens = {};
		test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ConstantValueSimpleSyntaxRule: Multiple tokens, does not throw") {
		list<Token> tokens = { Token::createIntegerToken("12345435"), Token::createIntegerToken("12345435") };
		list<Token> expectedTokens = { Token::createIntegerToken("12345435") };
		test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, not purely numeric, throws") {
		list<Token> tokens = { Token::createSemicolonToken() };
		testThrowsException(ConstantValueSimpleSyntaxRule(), tokens);
	}

		// -------------------- NameSimpleSyntaxRule --------------------
	SECTION("NameSimpleSyntaxRule: Exactly one token left, valid") {
		list<Token> tokens = { Token::createNameOrKeywordToken("soomevariable") };
		list<Token> expectedTokens = {};
		test(NameSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("NameSimpleSyntaxRule: Multiple tokens, does not throw") {
		list<Token> tokens = { Token::createNameOrKeywordToken("soomevariable"), Token::createNameOrKeywordToken("soomevariable") };
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("soomevariable") };
		test(NameSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("NameSimpleSyntaxRule: Exactly one token left, not name token, throws") {
		list<Token> tokens = { Token::createSemicolonToken() };
		testThrowsException(NameSimpleSyntaxRule(), tokens);
	}


		// -------------------- ReadSimpleSyntaxRule --------------------
	SECTION("ReadSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("othervariableonnextline") };
		test(ReadSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ReadSimpleSyntaxRule: Missing read token") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(ReadSimpleSyntaxRule(), tokens);
	}
	SECTION("ReadSimpleSyntaxRule: Missing name token") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(ReadSimpleSyntaxRule(), tokens);
	}
	SECTION("ReadSimpleSyntaxRule: Missing semicolon token") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createNameOrKeywordToken("othervariableonnextline")
		};
		testThrowsException(ReadSimpleSyntaxRule(), tokens);
	}

		// -------------------- PrintSimpleSyntaxRule --------------------
	SECTION("PrintSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createPrintToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("othervariableonnextline") };
		test(PrintSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("PrintSimpleSyntaxRule: Missing read token") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(PrintSimpleSyntaxRule(), tokens);
	}
	SECTION("PrintSimpleSyntaxRule: Missing name token") {
		list<Token> tokens = {
				Token::createPrintToken(),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(PrintSimpleSyntaxRule(), tokens);
	}
	SECTION("PrintSimpleSyntaxRule: Missing semicolon token") {
		list<Token> tokens = {
				Token::createPrintToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(PrintSimpleSyntaxRule(), tokens);
	}

		// -------------------- AssignSimpleSyntaxRule --------------------
	SECTION("AssignSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		list<Token> expectedTokens = { Token::createNameOrKeywordToken("othervariableonnextline") };
		test(AssignSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("AssignSimpleSyntaxRule: Missing = token") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(AssignSimpleSyntaxRule(), tokens);
	}
	SECTION("AssignSimpleSyntaxRule: Missing token after equal") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createSemicolonToken(),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(AssignSimpleSyntaxRule(), tokens);
	}
	SECTION("AssignSimpleSyntaxRule: Missing semicolon token") {
		list<Token> tokens = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createNameOrKeywordToken("othervariableonnextline"),
		};
		testThrowsException(AssignSimpleSyntaxRule(), tokens);
	}

		// -------------------- StatementListSimpleSyntaxRule --------------------
	SECTION("StatementListSimpleSyntaxRule: Consumes exactly correct tokens, using example with just an assign") {
		list<Token> tokens = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		list<Token> expectedTokens = {};
		test(StatementListSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("StatementListSimpleSyntaxRule: Missing {") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(StatementListSimpleSyntaxRule(), tokens);
	}
	SECTION("StatementListSimpleSyntaxRule: Missing }") {
		list<Token> tokens = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};
		testThrowsException(StatementListSimpleSyntaxRule(), tokens);
	}

	SECTION("StatementListSimpleSyntaxRule: Empty Statement list") {
		list<Token> tokens = {
		Token::createOpenCurlyBracketToken(),
		Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(StatementListSimpleSyntaxRule(), tokens);
	}

		// -------------------- ProcedureSimpleSyntaxRule --------------------
	SECTION("ProcedureSimpleSyntaxRule: Consumes exactly correct tokens, using example with just an assign") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		list<Token> expectedTokens = {};
		test(ProcedureSimpleSyntaxRule(), tokens, expectedTokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing procedure keyword") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing name") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing {") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}
	SECTION("ProcedureSimpleSyntaxRule: Missing }") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}

	SECTION("ProcedureSimpleSyntaxRule: empty procedure") {
		list<Token> tokens = {
		Token::createProcedureToken(),
			Token::createNameOrKeywordToken("procedureName"),
			Token::createOpenCurlyBracketToken(),
			Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
	}

	SECTION("IfSimpleSyntaxRule: no else") {
		list<Token> tokens = {
			Token::createIfToken(),
			Token::createOpenBracketToken(),
			Token::createNameOrKeywordToken("x"),
			Token::createOperatorToken(">"),
			Token::createNameOrKeywordToken("y"),
			Token::createCloseBracketToken(),
			Token::createThenToken(),
			Token::createOpenCurlyBracketToken(),
			Token::createPrintToken(),
			Token::createNameOrKeywordToken("x"),
			Token::createCloseCurlyBracketToken(),
		};
		testThrowsException(IfSimpleSyntaxRule(), tokens);
	}
 
		// -------------------- ProgramSimpleSyntaxRule --------------------
	SECTION("ProgramSimpleSyntaxRule: Consumes exactly correct tokens, using two procedures") {
		list<Token> tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("anotherProcedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		list<Token> expectedTokens = {};
		test(ProgramSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- OperatorSimpleSyntaxRule --------------------
	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createPlusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createMinusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createMultiplyToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createDivideToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createModulusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createModulusToken(),
		};
		list<Token> expectedTokens = {};
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

	SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
				Token::createPlusToken(),
				Token::createIntegerToken("2")
		};
		list<Token> expectedTokens = { Token::createIntegerToken("5"),
									   Token::createPlusToken(),
									   Token::createIntegerToken("2") };
		test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- ExpressionSimpleSyntaxRule --------------------
	SECTION("ExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("x"),
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
				Token::createPlusToken(),
				Token::createIntegerToken("2")
		};
		list<Token> expectedTokens = {};
		test(ExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- ExpressionSimpleSyntaxRule --------------------
	SECTION("ExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("y"),
				Token::createNameOrKeywordToken("z"), // Illegal operator
				Token::createIntegerToken("2"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createIntegerToken("10")
		};
		list<Token> expectedTokens = {};
		testThrowsException(ExpressionSimpleSyntaxRule(), tokens);
	}


		// -------------------- RelationalExpressionSimpleSyntaxRule --------------------
	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualityToken(),
				Token::createIntegerToken("1"),
		};
		list<Token> expectedTokens = {};
		test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- RelationalExpressionSimpleSyntaxRule --------------------
	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
		};
		list<Token> expectedTokens = {};
		test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- RelationalExpressionSimpleSyntaxRule --------------------
	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("y"),
		};
		list<Token> expectedTokens = { };
		test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- RelationalExpressionSimpleSyntaxRule --------------------
	SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createNameOrKeywordToken("y"),
		};
		list<Token> expectedTokens = { Token::createPlusToken(),
									   Token::createNameOrKeywordToken("y") };
		testThrowsException(RelationalExpressionSimpleSyntaxRule(), tokens);
	}


		// -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, just a relational operator test") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken()
		};
		list<Token> expectedTokens = {};
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, Not test") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createNotToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken()
		};
		list<Token> expectedTokens = {};
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, two relational expressions") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken()
		};
		list<Token> expectedTokens = {};
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, doesnt consume extra tokens") {
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCallToken(),
				Token::createNameOrKeywordToken("some random procedure")
		};
		list<Token> expectedTokens = { Token::createCallToken(),
									   Token::createNameOrKeywordToken("some random procedure") };
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
	SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, nested conditional expressions") {
		// ((v >= x) || (q >= x)) && (soomevariable >= x)
		list<Token> tokens = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCallToken(),
				Token::createNameOrKeywordToken("main")
		};
		list<Token> expectedTokens = { Token::createCallToken(),
									   Token::createNameOrKeywordToken("main") };
		test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- WhileSimpleSyntaxRule --------------------
	SECTION("WhileSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createWhileToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x")
		};
		list<Token> expectedTokens = { Token::createReadToken(),
									   Token::createNameOrKeywordToken("x") };
		test(WhileSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- WhileSimpleSyntaxRule --------------------
	SECTION("WhileSimpleSyntaxRule: Consumes exactly correct tokens, with a nested condition") {
		list<Token> tokens = {
				Token::createWhileToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x")
		};
		list<Token> expectedTokens = { Token::createReadToken(),
									   Token::createNameOrKeywordToken("x") };
		test(WhileSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- IfSimpleSyntaxRule --------------------
	SECTION("IfSimpleSyntaxRule: Consumes exactly correct tokens") {
		list<Token> tokens = {
				Token::createIfToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createThenToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createElseToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createPrintToken(),
				Token::createNameOrKeywordToken("x"),
		};
		list<Token> expectedTokens = { Token::createPrintToken(),
									   Token::createNameOrKeywordToken("x"), };
		test(IfSimpleSyntaxRule(), tokens, expectedTokens);
	}

		// -------------------- IfSimpleSyntaxRule --------------------
	SECTION("IfSimpleSyntaxRule: Consumes exactly correct tokens, with a nested condition") {
		list<Token> tokens = {
				Token::createIfToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createThenToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createElseToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createPrintToken(),
				Token::createNameOrKeywordToken("x"),
		};
		list<Token> expectedTokens = { Token::createPrintToken(),
									   Token::createNameOrKeywordToken("x"), };
		test(IfSimpleSyntaxRule(), tokens, expectedTokens);
	}
}

TEST_CASE("Parser: test ::generateChildRules") {

	auto test = [](SimpleSyntaxRule& rule, list<Token> tokensToConsume, vector<shared_ptr<SimpleSyntaxRule>> expectedChildren) {

		// ----- given -----
		// consume tokens and initialize rule
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);

		// ----- when ------
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// ----- then -----
		REQUIRE(childRules.size() == expectedChildren.size()); // same size
		for (int i = 0; i < childRules.size(); i++) {
			auto rule = childRules.at(i);
			auto otherRule = expectedChildren.at(i);
			REQUIRE((*rule).equals(*otherRule));
		}
	};

	// -------------------- ConstantValueSimpleSyntaxRule --------------------
	SECTION("ConstValueSimpleSyntaxRule: is terminal, no rules to be generated") {
		list<Token> tokensToConsume = { Token::createIntegerToken("1") };
		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {};
		test(ConstantValueSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- NameSimpleSyntaxRule --------------------
	SECTION("NameSimpleSyntaxRule: is terminal, no rules to be generated") {
		list<Token> tokensToConsume = { Token::createNameOrKeywordToken("a") };
		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {};
		test(NameSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- ReadSimpleSyntaxRule --------------------
	SECTION("ReadSimpleSyntaxRule: nested is the name keyword, should have name rule generated") {
		list<Token> tokensToConsume = {
				Token::createReadToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
		};
		shared_ptr<SimpleSyntaxRule> nameRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInNameRule = { Token::createNameOrKeywordToken("soomevariable") };
		nameRule->consumeTokens(tokensInNameRule);
		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				nameRule
		};
		test(ReadSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- PrintSimpleSyntaxRule --------------------
	SECTION("PrintSimpleSyntaxRule:") {
		list<Token> tokensToConsume = {
				Token::createPrintToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createSemicolonToken(),
		};
		shared_ptr<SimpleSyntaxRule> nameRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInNameRule = { Token::createNameOrKeywordToken("soomevariable") };
		nameRule->consumeTokens(tokensInNameRule);
		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				nameRule
		};
		test(PrintSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- AssignSimpleSyntaxRule --------------------
	SECTION("AssignSimpleSyntaxRule: nested is the lhs and rhs, should have name lhs and const rhs generated") {
		// tokens, an assign statement
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("soomevariable") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createIntegerToken("1"), };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				rhsRule
		};
		test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("AssignSimpleSyntaxRule: expression x = x + 1") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("soomevariable") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createNameOrKeywordToken("x"), Token::createPlusToken(), Token::createIntegerToken("1") };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				rhsRule
		};
		test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("AssignSimpleSyntaxRule: expression x = x + 1 - 3") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createPlusToken(),
				Token::createIntegerToken("1"),
				Token::createMinusToken(),
				Token::createIntegerToken("3"),
				Token::createSemicolonToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("soomevariable") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createNameOrKeywordToken("x"), Token::createPlusToken(), Token::createIntegerToken("1"), Token::createMinusToken(), Token::createIntegerToken("3"), };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				rhsRule
		};
		test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- StatementListSimpleSyntaxRule --------------------
	SECTION("StatementListSimpleSyntaxRule: nested assignment statement") {
		// tokens, an assign statement in a procedure
		list<Token> tokensToConsume = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// create child rules
		shared_ptr<SimpleSyntaxRule> childRule1 = shared_ptr<SimpleSyntaxRule>(new AssignSimpleSyntaxRule());
		list<Token> tokensInChildRule = {
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
		};
		childRule1->consumeTokens(tokensInChildRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				childRule1,
		};
		test(StatementListSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- ProcedureSimpleSyntaxRule --------------------
	SECTION("ProcedureSimpleSyntaxRule: nested assignment statement") {
		// tokens, an assign statement in a procedure
		list<Token> tokensToConsume = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> procedureName = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> procedureNameTokens = { Token::createNameOrKeywordToken("procedureName") };
		procedureName->consumeTokens(procedureNameTokens);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> statementList = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> statementListTokens = {
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		statementList->consumeTokens(statementListTokens);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				procedureName,
				statementList
		};
		test(ProcedureSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- ProgramSimpleSyntaxRule --------------------
	SECTION("ProgramSimpleSyntaxRule: two procedures") {
		// tokens, an assign statement in a procedure
		list<Token> tokensToConsume = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("anotherProcedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// create child procedures
		// 1. first procedure
		shared_ptr<SimpleSyntaxRule> procedure1 = shared_ptr<SimpleSyntaxRule>(new ProcedureSimpleSyntaxRule());
		list<Token> procedure1tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("procedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),

		};
		procedure1->consumeTokens(procedure1tokens);

		// 2. second procedure
		shared_ptr<SimpleSyntaxRule> procedure2 = shared_ptr<SimpleSyntaxRule>(new ProcedureSimpleSyntaxRule());
		list<Token> procedure2tokens = {
				Token::createProcedureToken(),
				Token::createNameOrKeywordToken("anotherProcedureName"),
				Token::createOpenCurlyBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createEqualsToken(),
				Token::createIntegerToken("1"),
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};
		procedure2->consumeTokens(procedure2tokens);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				procedure1,
				procedure2
		};
		test(ProcedureSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- ExpressionSimpleSyntaxRule --------------------
	SECTION("ExpressionSimpleSyntaxRule: x % 5") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("x"),
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("x") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createModulusToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createIntegerToken("5") };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ExpressionSimpleSyntaxRule: x % 5 + 6") {
		list<Token> tokensToConsume = {
				Token::createNameOrKeywordToken("x"),
				Token::createModulusToken(),
				Token::createIntegerToken("5"),
				Token::createPlusToken(),
				Token::createIntegerToken("6")
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createNameOrKeywordToken("x") };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createModulusToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createIntegerToken("5"), Token::createPlusToken(), Token::createIntegerToken("6") };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

		// -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
	SECTION("ConditionalExpressionSimpleSyntaxRule: x >= 5") {
		list<Token> tokensToConsume = {
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken()
		};

		// create relational rule
		shared_ptr<SimpleSyntaxRule> relationalRule = shared_ptr<SimpleSyntaxRule>(new RelationalExpressionSimpleSyntaxRule());
		list<Token> tokensRule = { Token::createNameOrKeywordToken("soomevariable"),Token::createGreaterThanEqualToken(),
								   Token::createNameOrKeywordToken("x"), };
		relationalRule->consumeTokens(tokensRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				relationalRule,
		};

		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: !(soomevariable >= x)") {
		list<Token> tokensToConsume = {
				Token::createNotToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
		};

		// create conditional rule
		shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { 
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
		};
		conRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				conRule
		};
		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: (soomevariable >= x) && (soomevariable >= x)") {
		list<Token> tokensToConsume = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken()
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { 
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken()
		};
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createAndToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { 
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken() };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("ConditionalExpressionSimpleSyntaxRule: ((v >= x) || (q >= x)) && ((v >= x) || (q >= x))") {
		list<Token> tokensToConsume = {
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createAndToken(),
				Token::createOpenBracketToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("v"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOrToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("q"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
				Token::createCloseBracketToken(),
		};

		// create lhs rule
		shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInLHSRule = { Token::createOpenBracketToken(),
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("v"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
										Token::createOrToken(),
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("q"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
										Token::createCloseBracketToken() };
		lhsRule->consumeTokens(tokensInLHSRule);

		// create operator rule
		shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
		list<Token> tokensInOperatorRule = { Token::createAndToken() };
		operatorRule->consumeTokens(tokensInOperatorRule);

		// create rhs rule
		shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInRHSRule = { Token::createOpenBracketToken(),
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("v"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
										Token::createOrToken(),
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("q"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken(),
										Token::createCloseBracketToken() };
		rhsRule->consumeTokens(tokensInRHSRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				lhsRule,
				operatorRule,
				rhsRule
		};
		test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("WhileSimpleSyntaxRule: standard condition") {
		list<Token> tokensToConsume = {
				Token::createWhileToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
		};

		// create conditional rule
		shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInConRule = { 
										Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken()};
		conRule->consumeTokens(tokensInConRule);

		// Create stmtList rule
		shared_ptr<SimpleSyntaxRule> stmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> tokensInStmtLstRule = { Token::createOpenCurlyBracketToken(),
											Token::createReadToken(),
											Token::createNameOrKeywordToken("x"),
											Token::createCloseCurlyBracketToken() };
		stmtLstRule->consumeTokens(tokensInStmtLstRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				conRule,
				stmtLstRule
		};
		test(WhileSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}

	SECTION("IfSimpleSyntaxRule: standard condition") {
		list<Token> tokensToConsume = {
				Token::createIfToken(),
				Token::createOpenBracketToken(),
				Token::createNameOrKeywordToken("soomevariable"),
				Token::createGreaterThanEqualToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseBracketToken(),
				Token::createThenToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
				Token::createElseToken(),
				Token::createOpenCurlyBracketToken(),
				Token::createReadToken(),
				Token::createNameOrKeywordToken("x"),
				Token::createCloseCurlyBracketToken(),
		};

		// create conditional rule
		shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
		list<Token> tokensInConRule = { Token::createOpenBracketToken(),
										Token::createNameOrKeywordToken("soomevariable"),
										Token::createGreaterThanEqualToken(),
										Token::createNameOrKeywordToken("x"),
										Token::createCloseBracketToken() };
		conRule->consumeTokens(tokensInConRule);

		// Create stmtList rule
		shared_ptr<SimpleSyntaxRule> thenStmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> tokensInThenStmtLstRule = { Token::createOpenCurlyBracketToken(),
												Token::createReadToken(),
												Token::createNameOrKeywordToken("x"),
												Token::createCloseCurlyBracketToken() };
		thenStmtLstRule->consumeTokens(tokensInThenStmtLstRule);

		shared_ptr<SimpleSyntaxRule> elseStmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
		list<Token> tokensInElseStmtLstRule = { Token::createOpenCurlyBracketToken(),
												Token::createReadToken(),
												Token::createNameOrKeywordToken("x"),
												Token::createCloseCurlyBracketToken() };
		elseStmtLstRule->consumeTokens(tokensInElseStmtLstRule);

		vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
				conRule,
				thenStmtLstRule,
				elseStmtLstRule
		};
		test(IfSimpleSyntaxRule(), tokensToConsume, expectedChildren);
	}
}


TEST_CASE("Parser: test ::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

	// -------------------- ConstantValueSimpleSyntaxRule --------------------
	SECTION("ConstantValueSimpleSyntaxRule : constructNode") {
		Token constantValueToken = Token::createIntegerToken("1");
		list<Token> tokensToConsume = { constantValueToken };
		ConstantValueSimpleSyntaxRule rule = ConstantValueSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create constant node
		shared_ptr<ASTNode> expectedASTNode(new ConstantValueASTNode(constantValueToken));

		test(rule, expectedASTNode);
	}
		// -------------------- NameSimpleSyntaxRule --------------------
	SECTION("NameSimpleSyntaxRule : constructNode") {
		Token variableValueToken = Token::createNameOrKeywordToken("1");
		list<Token> tokensToConsume = { variableValueToken };
		NameSimpleSyntaxRule rule = NameSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create variable node
		shared_ptr<ASTNode> expectedASTNode(new VariableASTNode(variableValueToken));

		test(rule, expectedASTNode);
	}

		// -------------------- ReadSimpleSyntaxRule --------------------
	SECTION("ReadSimpleSyntaxRule : constructNode") {
		// read soomevariable;
		Token readToken = Token::createReadToken();
		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { readToken,  variable, semiColon };

		ReadSimpleSyntaxRule rule = ReadSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create Read Node
		shared_ptr<ASTNode> expectedASTNode(new ReadASTNode(readToken));

		// Create variableNode
		shared_ptr<ASTNode> variableNode(new VariableASTNode(variable));

		expectedASTNode->addChild(variableNode);


		test(rule, expectedASTNode);
	}

		// -------------------- PrintSimpleSyntaxRule --------------------
	SECTION("PrintSimpleSyntaxRule : constructNode") {
		// read soomevariable;
		Token printToken = Token::createPrintToken();
		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { printToken,  variable, semiColon };

		PrintSimpleSyntaxRule rule = PrintSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create Read Node
		shared_ptr<ASTNode> expectedASTNode(new PrintASTNode(printToken));

		// Create variableNode
		shared_ptr<ASTNode> variableNode(new VariableASTNode(variable));

		expectedASTNode->addChild(variableNode);


		test(rule, expectedASTNode);
	}

		// -------------------- AssignSimpleSyntaxRule --------------------
	SECTION("AssignSimpleSyntaxRule : constructNode") {
		// x = 5;
		Token leftHandSideToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token rightHandSideToken = Token::createIntegerToken("5");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { leftHandSideToken,  equalsToken, rightHandSideToken, semiColon };

		// Create rule
		AssignSimpleSyntaxRule rule = AssignSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new AssignASTNode(equalsToken));

		// Create LHS
		shared_ptr<ASTNode> variableNode(new VariableASTNode(leftHandSideToken));

		// Create RHS
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(rightHandSideToken));

		expectedASTNode->addChild(variableNode);
		expectedASTNode->addChild(constantNode);

		test(rule, expectedASTNode);
	}

	SECTION("AssignSimpleSyntaxRule : constructNode -> x = x + 1") {
		// x = x + 1;
		Token leftHandSideToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token rightXToken = Token::createNameOrKeywordToken("x");
		Token plusToken = Token::createPlusToken();
		Token rightOneToken = Token::createIntegerToken("1");
		Token semiColon = Token::createSemicolonToken();
		list<Token> tokensToConsume = { leftHandSideToken,  equalsToken, rightXToken, plusToken, rightOneToken, semiColon };

		// Create rule
		AssignSimpleSyntaxRule rule = AssignSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new AssignASTNode(equalsToken));

		// Create LHS
		shared_ptr<ASTNode> variableNode(new VariableASTNode(leftHandSideToken));
		shared_ptr<ASTNode> rhsXNode(new VariableASTNode(rightXToken));

		// Create expression
		shared_ptr<ASTNode> expressionNode(new ExpressionASTNode(plusToken));

		// Create RHS
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(rightOneToken));

		expectedASTNode->addChild(variableNode);
		expectedASTNode->addChild(expressionNode);

		expressionNode->addChild(rhsXNode);
		expressionNode->addChild(constantNode);

		test(rule, expectedASTNode);
	}

	SECTION("AssignSimpleSyntaxRule : constructNode -> x = x + 1 + (1 + x)") {
		// x = x + 1;
		Token leftHandSideToken = Token::createNameOrKeywordToken("x");
		Token equalsToken = Token::createEqualsToken();
		Token rightXToken = Token::createNameOrKeywordToken("x");
		Token plusToken = Token::createPlusToken();
		Token rightOneToken = Token::createIntegerToken("1");
		Token semiColon = Token::createSemicolonToken();
		Token openBracket = Token::createOpenBracketToken();
		Token closedBracket = Token::createCloseBracketToken();
		list<Token> tokensToConsume = { leftHandSideToken,  equalsToken,
										rightXToken, plusToken, rightOneToken, plusToken,
										openBracket, rightOneToken, plusToken, rightXToken, closedBracket, semiColon };

		// Create rule
		AssignSimpleSyntaxRule rule = AssignSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new AssignASTNode(equalsToken));

		// Create LHS
		shared_ptr<ASTNode> variableNode(new VariableASTNode(leftHandSideToken));
		shared_ptr<ASTNode> rhsXOutNode(new VariableASTNode(rightXToken));
		shared_ptr<ASTNode> rhsXInNode(new VariableASTNode(rightXToken));

		// Create expressions
		shared_ptr<ASTNode> expressionOneNode(new ExpressionASTNode(plusToken));
		shared_ptr<ASTNode> expressionTwoNode(new ExpressionASTNode(plusToken));
		shared_ptr<ASTNode> expressionBracketNode(new ExpressionASTNode(plusToken));
		shared_ptr<ASTNode> bracketASTNode(new BracketsASTNode(Token::getPlaceHolderToken()));

		// Create RHS
		shared_ptr<ASTNode> constantOutNode(new ConstantValueASTNode(rightOneToken));
		shared_ptr<ASTNode> constantInNode(new ConstantValueASTNode(rightOneToken));

		expectedASTNode->addChild(variableNode);
		expectedASTNode->addChild(expressionOneNode);

		expressionOneNode->addChild(rhsXOutNode);
		expressionOneNode->addChild(expressionTwoNode);

		expressionTwoNode->addChild(constantOutNode);
		expressionTwoNode->addChild(bracketASTNode);

		bracketASTNode->addChild(expressionBracketNode);

		expressionBracketNode->addChild(constantInNode);
		expressionBracketNode->addChild(rhsXInNode);

		test(rule, expectedASTNode);
	}

		// -------------------- StatementListSimpleSyntaxRule --------------------
	SECTION("StatementListSimpleSyntaxRule : constructNode") {

		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token equalsToken = Token::createEqualsToken();
		Token constantToken = Token::createIntegerToken("1");

		list<Token> tokensToConsume = {
				Token::createOpenCurlyBracketToken(),
				variable,
				equalsToken,
				constantToken,
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// Create rule
		StatementListSimpleSyntaxRule rule = StatementListSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		//Create StmtList node
		shared_ptr<ASTNode> expectedASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		//Create Assign node
		shared_ptr<ASTNode> assignASTNode(new AssignASTNode(equalsToken));

		// Create LHS
		shared_ptr<ASTNode> variableNode(new VariableASTNode(variable));

		// Create RHS
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));

		assignASTNode->addChild(variableNode);
		assignASTNode->addChild(constantNode);

		expectedASTNode->addChild(assignASTNode);

		test(rule, expectedASTNode);
	}

		// -------------------- ProcedureSimpleSyntaxRule --------------------
	SECTION("ProcedureSimpleSyntaxRule : constructNode") {
		/*
			procedure procedureName {
				soomevariable = 1;
			}
		*/

		Token procedureToken = Token::createProcedureToken();
		Token procedureName = Token::createNameOrKeywordToken("procedureName");
		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token equalsToken = Token::createEqualsToken();
		Token constantToken = Token::createIntegerToken("1");

		list<Token> tokensToConsume = {
				procedureToken,
				procedureName,
				Token::createOpenCurlyBracketToken(),
				variable,
				equalsToken,
				constantToken,
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// Create rule
		ProcedureSimpleSyntaxRule rule = ProcedureSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create procedure node
		shared_ptr<ASTNode> expectedASTNode(new ProcedureASTNode(procedureName));

		// Create stmtlst node
		shared_ptr<ASTNode> stmtLstASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// Create assign node
		shared_ptr<ASTNode> assignASTNode(new AssignASTNode(equalsToken));

		// Create variable node
		shared_ptr<ASTNode> variableNode(new VariableASTNode(variable));

		// Create constant node
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));

		assignASTNode->addChild(variableNode);
		assignASTNode->addChild(constantNode);

		stmtLstASTNode->addChild(assignASTNode);

		expectedASTNode->addChild(stmtLstASTNode);

		test(rule, expectedASTNode);
	}

		// -------------------- ProgramSimpleSyntaxRule --------------------
	SECTION("ProgramSimpleSyntaxRule : constructNode") {
		/*
			procedure procedureName {
			1.    soomevariable = 1;
			}

			procedure anotherProcedureName {
			2.    variable = 1;
			}
		*/

		Token procedureToken = Token::createProcedureToken();
		Token procedureName = Token::createNameOrKeywordToken("procedureName");
		Token anotherProcedureName = Token::createNameOrKeywordToken("anotherProcedureName");
		Token variable = Token::createNameOrKeywordToken("soomevariable");
		Token anotherVariable = Token::createNameOrKeywordToken("variable");
		Token equalsToken = Token::createEqualsToken();
		Token constantToken = Token::createIntegerToken("1");

		list<Token> tokensToConsume = {
				procedureToken,
				procedureName,
				Token::createOpenCurlyBracketToken(),
				variable,
				equalsToken,
				constantToken,
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
				procedureToken,
				anotherProcedureName,
				Token::createOpenCurlyBracketToken(),
				anotherVariable,
				equalsToken,
				constantToken,
				Token::createSemicolonToken(),
				Token::createCloseCurlyBracketToken(),
		};

		// Create rule
		ProgramSimpleSyntaxRule rule = ProgramSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create expected ASTNode
		shared_ptr<ASTNode> expectedASTNode(new ProgramASTNode({ Token::createProgramToken() }));

		// Create first procedure node
		shared_ptr<ASTNode> firstProcedureNode(new ProcedureASTNode(procedureName));
		// Create second procedure node
		shared_ptr<ASTNode> secondProcedureNode(new ProcedureASTNode(anotherProcedureName));

		// Create firstStmtlst node
		shared_ptr<ASTNode> firstStmtLstASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// Create secondStmtlst node
		shared_ptr<ASTNode> secondStmtLstASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

		// Create first assign node
		shared_ptr<ASTNode> firstAssignASTNode(new AssignASTNode(equalsToken));

		// Create first assign node
		shared_ptr<ASTNode> secondAssignASTNode(new AssignASTNode(equalsToken));


		// Create first variable node
		shared_ptr<ASTNode> variableNode(new VariableASTNode(variable));


		// Create second variable node
		shared_ptr<ASTNode> anotherVariableNode(new VariableASTNode(anotherVariable));


		// Create constant node
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));


		// Create another constant node
		shared_ptr<ASTNode> anotherConstantNode(new ConstantValueASTNode(constantToken));


		// Construct first procedure

		// add children to assign node
		firstAssignASTNode->addChild(variableNode);
		firstAssignASTNode->addChild(constantNode);

		// add assign node to stmtlst
		firstStmtLstASTNode->addChild(firstAssignASTNode);

		// Add name childs for procedures
		firstProcedureNode->addChild(firstStmtLstASTNode);

		// Construct first procedure

		// add children to second assign node
		secondAssignASTNode->addChild(anotherVariableNode);
		secondAssignASTNode->addChild(anotherConstantNode);

		// add assign node to stmtlst
		secondStmtLstASTNode->addChild(secondAssignASTNode);

		// Add name childs for procedures
		secondProcedureNode->addChild(secondStmtLstASTNode);


		// Add to program node
		expectedASTNode->addChild(firstProcedureNode);
		expectedASTNode->addChild(secondProcedureNode);

		test(rule, expectedASTNode);
	}

		// -------------------- OperatorSimpleSyntaxRule --------------------
	SECTION("OperatorSimpleSyntaxRule : constructNode") {
		// x = 5;
		Token addToken = Token::createPlusToken();
		list<Token> tokensToConsume = { addToken };

		// Create rule
		OperatorSimpleSyntaxRule rule = OperatorSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(addToken));

		test(rule, expectedASTNode);
	}

		// -------------------- ExpressionSimpleSyntaxRule --------------------
	SECTION("ExpressionSimpleSyntaxRule : constructNode") {
		// x % 5;
		Token xToken = Token::createNameOrKeywordToken("x");
		Token modToken = Token::createModulusToken();
		Token fiveToken = Token::createIntegerToken("5");
		list<Token> tokensToConsume = { xToken, modToken, fiveToken };

		// Create rule
		ExpressionSimpleSyntaxRule rule = ExpressionSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(modToken));
		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(fiveToken));

		expectedASTNode->addChild(xNode);
		expectedASTNode->addChild(constantNode);

		test(rule, expectedASTNode);
	}

		// -------------------- RelationalExpressionSimpleSyntaxRule --------------------
	SECTION("RelationalExpressionSimpleSyntaxRule : constructNode") {
		// x >= y;
		Token xToken = Token::createNameOrKeywordToken("x");
		Token greaterToken = Token::createGreaterThanEqualToken();
		Token yToken = Token::createNameOrKeywordToken("y");
		list<Token> tokensToConsume = { xToken, greaterToken, yToken };

		// Create rule
		RelationalExpressionSimpleSyntaxRule rule = RelationalExpressionSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(greaterToken));
		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));

		expectedASTNode->addChild(xNode);
		expectedASTNode->addChild(yNode);

		test(rule, expectedASTNode);
	}

		// -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
	SECTION("ConditionalExpressionSimpleSyntaxRule : constructNode") {
		// (x >= y) && (x >= y);
		Token openBracket = Token::createOpenBracketToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token closedBracket = Token::createCloseBracketToken();
		Token andToken = Token::createAndToken();

		list<Token> tokensToConsume = { openBracket, openBracket, variableX, greaterThan, variableY, closedBracket,
										andToken,
										openBracket, variableX, greaterThan, variableY, closedBracket, closedBracket };

		// Create rule
		ConditionalExpressionSimpleSyntaxRule rule = ConditionalExpressionSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create assign node
		shared_ptr<ASTNode> expectedASTNode(new ExpressionASTNode(andToken));

		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> greaterThanOneNode(new ExpressionASTNode(greaterThan));

		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yTwoNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> greaterThanTwoNode(new ExpressionASTNode(greaterThan));


		expectedASTNode->addChild(greaterThanOneNode);
		expectedASTNode->addChild(greaterThanTwoNode);

		greaterThanOneNode->addChild(xOneNode);
		greaterThanOneNode->addChild(yOneNode);

		greaterThanTwoNode->addChild(xTwoNode);
		greaterThanTwoNode->addChild(yTwoNode);

		test(rule, expectedASTNode);
	}

		// -------------------- WhileSimpleSyntaxRule --------------------
	SECTION("WhileSimpleSyntaxRule : constructNode") {
		// while (x > y) then { read x; } else  { read y;}
		Token whileToken = Token::createWhileToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token semiColonToken = Token::createSemicolonToken();

		list<Token> tokensToConsume = { Token::createWhileToken(),
										Token::createOpenBracketToken(),
										variableX,
										greaterThan,
										variableY,
										Token::createCloseBracketToken(),
										Token::createOpenCurlyBracketToken(),
										readToken,
										variableX,
										semiColonToken,
										Token::createCloseCurlyBracketToken(), };

		// Create rule
		WhileSimpleSyntaxRule rule = WhileSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create while node
		shared_ptr<ASTNode> expectedASTNode(new WhileASTNode(whileToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create stmtlst node
		shared_ptr<ASTNode> stmtLstNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

		readNode->addChild(xTwoNode);
		stmtLstNode->addChild(readNode);

		expectedASTNode->addChild(condNode);
		expectedASTNode->addChild(stmtLstNode);

		test(rule, expectedASTNode);
	}

		// -------------------- IfSimpleSyntaxRule --------------------
	SECTION("IfSimpleSyntaxRule : constructNode") {
		// if (x > y) then { read x; } else  { read y;}
		Token ifToken = Token::createIfToken();
		Token variableX = Token::createNameOrKeywordToken("x");
		Token greaterThan = Token::createGreaterThanEqualToken();
		Token variableY = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token semiColonToken = Token::createSemicolonToken();

		list<Token> tokensToConsume = { ifToken,
										Token::createOpenBracketToken(),
										variableX,
										greaterThan,
										variableY,
										Token::createCloseBracketToken(),
										Token::createThenToken(),
										Token::createOpenCurlyBracketToken(),
										readToken,
										variableX,
										semiColonToken,
										Token::createCloseCurlyBracketToken(),
										Token::createElseToken(),
										Token::createOpenCurlyBracketToken(),
										readToken,
										variableY,
										semiColonToken,
										Token::createCloseCurlyBracketToken(), };

		// Create rule
		IfSimpleSyntaxRule rule = IfSimpleSyntaxRule();
		list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
		vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

		// Create while node
		shared_ptr<ASTNode> expectedASTNode(new IfASTNode(ifToken));

		// Create condition node
		shared_ptr<ASTNode> xOneNode(new VariableASTNode(variableX));
		shared_ptr<ASTNode> yOneNode(new VariableASTNode(variableY));
		shared_ptr<ASTNode> condNode(new ExpressionASTNode(greaterThan));

		condNode->addChild(xOneNode);
		condNode->addChild(yOneNode);

		// Create then stmtlst node
		shared_ptr<ASTNode> stmtLstOneNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readOneNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

		// Create else stmtlst node
		shared_ptr<ASTNode> stmtLstTwoNode(new StatementListASTNode(Token::createPlaceholderToken()));

		shared_ptr<ASTNode> readTwoNode(new ReadASTNode(Token::createReadToken()));
		shared_ptr<ASTNode> yTwoNode(new VariableASTNode(variableY));

		readOneNode->addChild(xTwoNode);
		stmtLstOneNode->addChild(readOneNode);

		readTwoNode->addChild(yTwoNode);
		stmtLstTwoNode->addChild(readTwoNode);

		expectedASTNode->addChild(condNode);
		expectedASTNode->addChild(stmtLstOneNode);
		expectedASTNode->addChild(stmtLstTwoNode);

		test(rule, expectedASTNode);
	}
};

TEST_CASE("ProgramSimpleSyntaxRule test ::setLineNumber") {
	/*
	   procedure procedureName {
	   1.    soomevariable = 1;
	   }
	*/

	auto test = [](shared_ptr<ASTNode> toSet, shared_ptr<ASTNode> expected) {

		ProgramSimpleSyntaxRule rule = ProgramSimpleSyntaxRule();

		rule.setASTLineNumbers(toSet, 1);

		REQUIRE(toSet->equals(expected));

	};

	Token procedureToken = Token::createProcedureToken();
	Token procedureName = Token::createNameOrKeywordToken("procedureName");
	Token variable = Token::createNameOrKeywordToken("soomevariable");
	Token equalsToken = Token::createEqualsToken();
	Token constantToken = Token::createIntegerToken("1");

	// Create expected ASTNode
	shared_ptr<ASTNode> expectedASTNode(new ProgramASTNode({ Token::createProgramToken() }));

	// Create ASTNode with no line numbers
	shared_ptr<ASTNode> toSet(new ProgramASTNode({ Token::createProgramToken() }));

	// Create first procedure node
	shared_ptr<ASTNode> firstProcedureNode(new ProcedureASTNode(procedureName));

	// Create first procedure node w/o line numbers
	shared_ptr<ASTNode> toSetProcedureNode(new ProcedureASTNode(procedureName));

	// Create first variable node
	shared_ptr<ASTNode> variableNode(new VariableASTNode(variable));
	variableNode->setLineNumber(1);

	// Create first variable node w/o Line numbers
	shared_ptr<ASTNode> toSetVariableNode(new VariableASTNode(variable));

	// Create constant node
	shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));
	constantNode->setLineNumber(1);

	// Create constant node w/o line numbers
	shared_ptr<ASTNode> toSetConstantNode(new ConstantValueASTNode(constantToken));

	// Create firstStmtlst node
	shared_ptr<ASTNode> firstStmtLstASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

	// Create firstStmtlst node w/o line numbers
	shared_ptr<ASTNode> toSetStmtLstASTNode(new StatementListASTNode(Token::createPlaceholderToken()));

	// Create first assign node
	shared_ptr<ASTNode> firstAssignASTNode(new AssignASTNode(equalsToken));
	firstAssignASTNode->setLineNumber(1);

	// Create first assign node w/o line numbers
	shared_ptr<ASTNode> toSetAssignASTNode(new AssignASTNode(equalsToken));

	// add children to assign node
	firstAssignASTNode->addChild(variableNode);
	firstAssignASTNode->addChild(constantNode);

	toSetAssignASTNode->addChild(toSetVariableNode);
	toSetAssignASTNode->addChild(toSetConstantNode);



	// add assign node to stmtlst
	firstStmtLstASTNode->addChild(firstAssignASTNode);

	toSetStmtLstASTNode->addChild(toSetAssignASTNode);

	// Add name childs for procedures
	firstProcedureNode->addChild(firstStmtLstASTNode);

	toSetProcedureNode->addChild(toSetStmtLstASTNode);

	expectedASTNode->addChild(firstProcedureNode);
	toSet->addChild(toSetProcedureNode);

	test(toSet, expectedASTNode);

}



// =============== INTEGRATION TEST FOR RULES ====================