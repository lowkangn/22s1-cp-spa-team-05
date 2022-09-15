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
        list<Token> tokens = { Token("1", TokenType::INTEGER) };
        list<Token> expectedTokens = {};
        test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, very large number") {
        list<Token> tokens = { Token("12345435", TokenType::INTEGER) };
        list<Token> expectedTokens = {};
        test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("ConstantValueSimpleSyntaxRule: Multiple tokens, does not throw") {
        list<Token> tokens = { Token("12345435", TokenType::INTEGER), Token("12345435", TokenType::INTEGER) };
        list<Token> expectedTokens = { Token("12345435", TokenType::INTEGER) };
        test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("ConstantValueSimpleSyntaxRule: Exactly one token left, not purely numeric, throws") {
        list<Token> tokens = { Token(";", TokenType::DELIMITER) };
        testThrowsException(ConstantValueSimpleSyntaxRule(), tokens);
    }

    // -------------------- NameSimpleSyntaxRule --------------------
    SECTION("NameSimpleSyntaxRule: Exactly one token left, valid") {
        list<Token> tokens = { Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
        list<Token> expectedTokens = {};
        test(NameSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("NameSimpleSyntaxRule: Multiple tokens, does not throw") {
        list<Token> tokens = { Token("soomevariable", TokenType::NAME_OR_KEYWORD), Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
        list<Token> expectedTokens = { Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
        test(NameSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("NameSimpleSyntaxRule: Exactly one token left, not name token, throws") {
        list<Token> tokens = { Token(";", TokenType::DELIMITER) };
        testThrowsException(NameSimpleSyntaxRule(), tokens);
    }


    // -------------------- ReadSimpleSyntaxRule --------------------
    SECTION("ReadSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = { Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD) };
        test(ReadSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("ReadSimpleSyntaxRule: Missing read token") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(ReadSimpleSyntaxRule(), tokens);
    }
    SECTION("ReadSimpleSyntaxRule: Missing name token") {
        list<Token> tokens = {
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(ReadSimpleSyntaxRule(), tokens);
    }
    SECTION("ReadSimpleSyntaxRule: Missing semicolon token") {
        list<Token> tokens = {
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(ReadSimpleSyntaxRule(), tokens);
    }

    // -------------------- PrintSimpleSyntaxRule --------------------
    SECTION("PrintSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = { Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD) };
        test(PrintSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("PrintSimpleSyntaxRule: Missing read token") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(PrintSimpleSyntaxRule(), tokens);
    }
    SECTION("PrintSimpleSyntaxRule: Missing name token") {
        list<Token> tokens = {
            Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(PrintSimpleSyntaxRule(), tokens);
    }
    SECTION("PrintSimpleSyntaxRule: Missing semicolon token") {
        list<Token> tokens = {
            Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(PrintSimpleSyntaxRule(), tokens);
    }

    // -------------------- AssignSimpleSyntaxRule --------------------
    SECTION("AssignSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = { Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD) };
        test(AssignSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("AssignSimpleSyntaxRule: Missing = token") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(AssignSimpleSyntaxRule(), tokens);
    }
    SECTION("AssignSimpleSyntaxRule: Missing token after equal") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(AssignSimpleSyntaxRule(), tokens);
    }
    SECTION("AssignSimpleSyntaxRule: Missing semicolon token") {
        list<Token> tokens = {
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token("othervariableonnextline", TokenType::NAME_OR_KEYWORD),
        };
        testThrowsException(AssignSimpleSyntaxRule(), tokens);
    }

    // -------------------- StatementListSimpleSyntaxRule --------------------
    SECTION("StatementListSimpleSyntaxRule: Consumes exactly correct tokens, using example with just an assign") {
        list<Token> tokens = {
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };
        list<Token> expectedTokens = {};
        test(StatementListSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("StatementListSimpleSyntaxRule: Missing {") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };
        testThrowsException(StatementListSimpleSyntaxRule(), tokens);
    }
    SECTION("StatementListSimpleSyntaxRule: Missing }") {
        list<Token> tokens = {
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
        };
        testThrowsException(StatementListSimpleSyntaxRule(), tokens);
    }

    // -------------------- ProcedureSimpleSyntaxRule --------------------
    SECTION("ProcedureSimpleSyntaxRule: Consumes exactly correct tokens, using example with just an assign") {
        list<Token> tokens = {
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };
        list<Token> expectedTokens = {};
        test(ProcedureSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("ProcedureSimpleSyntaxRule: Missing procedure keyword") {
        list<Token> tokens = {
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };
        testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
    }
    SECTION("ProcedureSimpleSyntaxRule: Missing name") {
        list<Token> tokens = {
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };
        testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
    }
    SECTION("ProcedureSimpleSyntaxRule: Missing {") {
        list<Token> tokens = {
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };
        testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
    }
    SECTION("ProcedureSimpleSyntaxRule: Missing }") {
        list<Token> tokens = {
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
        };
        testThrowsException(ProcedureSimpleSyntaxRule(), tokens);
    }

    // -------------------- ProgramSimpleSyntaxRule --------------------
    SECTION("ProgramSimpleSyntaxRule: Consumes exactly correct tokens, using two procedures") {
        list<Token> tokens = {
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("anotherProcedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };
        list<Token> expectedTokens = {};
        test(ProgramSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- OperatorSimpleSyntaxRule --------------------
    SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("+", TokenType::OPERATOR),
        };
        list<Token> expectedTokens = {};
        test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
    }

    SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("-", TokenType::OPERATOR),
        };
        list<Token> expectedTokens = {};
        test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
    }

    SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("*", TokenType::OPERATOR),
        };
        list<Token> expectedTokens = {};
        test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
    }

    SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("/", TokenType::OPERATOR),
        };
        list<Token> expectedTokens = {};
        test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
    }

    SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("%", TokenType::OPERATOR),
        };
        list<Token> expectedTokens = {};
        test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
    }

    SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("%", TokenType::OPERATOR),
        };
        list<Token> expectedTokens = {};
        test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
    }

    SECTION("OperatorSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("%", TokenType::OPERATOR),
            Token("5", TokenType::INTEGER),
            Token("+", TokenType::OPERATOR),
            Token("2", TokenType::INTEGER)
        };
        list<Token> expectedTokens = { Token("5", TokenType::INTEGER),
            Token("+", TokenType::OPERATOR),
            Token("2", TokenType::INTEGER) };
        test(OperatorSimpleSyntaxRule(), tokens, expectedTokens);
    }
  
    // -------------------- ExpressionSimpleSyntaxRule --------------------
    SECTION("ExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token("%", TokenType::OPERATOR),
            Token("5", TokenType::INTEGER),
            Token("+", TokenType::OPERATOR),
            Token("2", TokenType::INTEGER)
        };
        list<Token> expectedTokens = {};
        test(ExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- ExpressionSimpleSyntaxRule --------------------
    SECTION("ExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("y", TokenType::NAME_OR_KEYWORD),
            Token("z", TokenType::NAME_OR_KEYWORD), // Illegal operator
            Token("2", TokenType::INTEGER),
            Token("+", TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token("+", TokenType::OPERATOR),
            Token("10", TokenType::INTEGER)
        };
        list<Token> expectedTokens = {};
        testThrowsException(ExpressionSimpleSyntaxRule(), tokens);
    }

   
    // -------------------- RelationalExpressionSimpleSyntaxRule --------------------
    SECTION("ProgramSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUALITY_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
        };
        list<Token> expectedTokens = {};
        test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- RelationalExpressionSimpleSyntaxRule --------------------
    SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = {};
        test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- RelationalExpressionSimpleSyntaxRule --------------------
    SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token("+", TokenType::OPERATOR),
            Token("y", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = { Token("+", TokenType::OPERATOR),
            Token("y", TokenType::NAME_OR_KEYWORD) };
        test(RelationalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- RelationalExpressionSimpleSyntaxRule --------------------
    SECTION("RelationalExpressionSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token("+", TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token("+", TokenType::OPERATOR),
            Token("y", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = { Token("+", TokenType::OPERATOR),
            Token("y", TokenType::NAME_OR_KEYWORD) };
        testThrowsException(RelationalExpressionSimpleSyntaxRule(), tokens);
    }

   
    // -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
    SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, just a relational operator test") {
        list<Token> tokens = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = {};
        test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
    SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, Not test") {
        list<Token> tokens = {
            Token(NOT, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER)
        };
        list<Token> expectedTokens = {};
        test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
    SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, two relational expressions") {
        list<Token> tokens = {
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(AND, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER)
        };
        list<Token> expectedTokens = {};
        test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
    SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, doesnt consume extra tokens") {
        list<Token> tokens = {
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(AND, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CALL_KEYWORD, TokenType::DELIMITER),
            Token("some random procedure", TokenType::NAME_OR_KEYWORD)
        };
        list<Token> expectedTokens = { Token(CALL_KEYWORD, TokenType::DELIMITER),
            Token("some random procedure", TokenType::NAME_OR_KEYWORD) };
        test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- ConditionalExpressionSimpleSyntaxRule --------------------
    SECTION("ConditionalExpressionSimpleSyntaxRule: Consumes exactly correct tokens, nested conditional expressions") {
        // ((v >= x) || (q >= x)) && (soomevariable >= x)
        list<Token> tokens = {
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(AND, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CALL_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("main", TokenType::NAME_OR_KEYWORD)
        };
        list<Token> expectedTokens = { Token(CALL_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("main", TokenType::NAME_OR_KEYWORD) };
        test(ConditionalExpressionSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- WhileSimpleSyntaxRule --------------------
    SECTION("WhileSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD)
        };
        list<Token> expectedTokens = { Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD) };
        test(WhileSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- WhileSimpleSyntaxRule --------------------
    SECTION("WhileSimpleSyntaxRule: Consumes exactly correct tokens, with a nested condition") {
        list<Token> tokens = {
            Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD)
        };
        list<Token> expectedTokens = { Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD) };
        test(WhileSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- IfSimpleSyntaxRule --------------------
    SECTION("IfSimpleSyntaxRule: Consumes exactly correct tokens") {
        list<Token> tokens = {
            Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(THEN_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(ELSE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = { Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD), };
        test(IfSimpleSyntaxRule(), tokens, expectedTokens);
    }

    // -------------------- IfSimpleSyntaxRule --------------------
    SECTION("IfSimpleSyntaxRule: Consumes exactly correct tokens, with a nested condition") {
        list<Token> tokens = {
            Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(THEN_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(ELSE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
        };
        list<Token> expectedTokens = { Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD), };
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
        list<Token> tokensToConsume = { Token("1", TokenType::INTEGER) };
        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {};
        test(ConstantValueSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }

    // -------------------- NameSimpleSyntaxRule --------------------
    SECTION("NameSimpleSyntaxRule: is terminal, no rules to be generated") {
        list<Token> tokensToConsume = { Token("a", TokenType::NAME_OR_KEYWORD) };
        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {};
        test(NameSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }

    // -------------------- ReadSimpleSyntaxRule --------------------
    SECTION("ReadSimpleSyntaxRule: nested is the name keyword, should have name rule generated") {
        list<Token> tokensToConsume = {
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
        };
        shared_ptr<SimpleSyntaxRule> nameRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> tokensInNameRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
        nameRule->consumeTokens(tokensInNameRule);
        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
            nameRule
        };
        test(ReadSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }

    // -------------------- PrintSimpleSyntaxRule --------------------
    SECTION("PrintSimpleSyntaxRule:") {
        list<Token> tokensToConsume = {
            Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(SEMI_COLON, TokenType::DELIMITER),
        };
        shared_ptr<SimpleSyntaxRule> nameRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> tokensInNameRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
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
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> tokensInLHSRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
        lhsRule->consumeTokens(tokensInLHSRule);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token("1", TokenType::INTEGER), };
        rhsRule->consumeTokens(tokensInRHSRule);

        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
            lhsRule,
            rhsRule
        };
        test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }

    SECTION("AssignSimpleSyntaxRule: expression x = x + 1") {
        list<Token> tokensToConsume = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(PLUS_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> tokensInLHSRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
        lhsRule->consumeTokens(tokensInLHSRule);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token("x", TokenType::NAME_OR_KEYWORD), Token(PLUS_OPERATOR, TokenType::OPERATOR), Token("1", TokenType::INTEGER) };
        rhsRule->consumeTokens(tokensInRHSRule);

        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
            lhsRule,
            rhsRule
        };
        test(AssignSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }

    SECTION("AssignSimpleSyntaxRule: expression x = x + 1 - 3") {
        list<Token> tokensToConsume = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(PLUS_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),            
            Token(MINUS_OPERATOR, TokenType::OPERATOR),
            Token("3", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> tokensInLHSRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD) };
        lhsRule->consumeTokens(tokensInLHSRule);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token("x", TokenType::NAME_OR_KEYWORD), Token(PLUS_OPERATOR, TokenType::OPERATOR), Token("1", TokenType::INTEGER), Token(MINUS_OPERATOR, TokenType::OPERATOR), Token("3", TokenType::INTEGER), };
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
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // create child rules
        shared_ptr<SimpleSyntaxRule> childRule1 = shared_ptr<SimpleSyntaxRule>(new AssignSimpleSyntaxRule());
        list<Token> tokensInChildRule = {
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
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
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> procedureName = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> procedureNameTokens = { Token("procedureName", TokenType::NAME_OR_KEYWORD) };
        procedureName->consumeTokens(procedureNameTokens);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> statementList = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
        list<Token> statementListTokens = {
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
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
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("anotherProcedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // create child procedures
        // 1. first procedure
        shared_ptr<SimpleSyntaxRule> procedure1 = shared_ptr<SimpleSyntaxRule>(new ProcedureSimpleSyntaxRule());
        list<Token> procedure1tokens = {
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("procedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),

        };
        procedure1->consumeTokens(procedure1tokens);

        // 2. second procedure
        shared_ptr<SimpleSyntaxRule> procedure2 = shared_ptr<SimpleSyntaxRule>(new ProcedureSimpleSyntaxRule());
        list<Token> procedure2tokens = {
            Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("anotherProcedureName", TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("1", TokenType::INTEGER),
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
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
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token("%", TokenType::OPERATOR),
            Token("5", TokenType::INTEGER),
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> tokensInLHSRule = { Token("x", TokenType::NAME_OR_KEYWORD) };
        lhsRule->consumeTokens(tokensInLHSRule);

        // create operator rule
        shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
        list<Token> tokensInOperatorRule = { Token("%", TokenType::OPERATOR) };
        operatorRule->consumeTokens(tokensInOperatorRule);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConstantValueSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token("5", TokenType::INTEGER) };
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
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token("%", TokenType::OPERATOR),
            Token("5", TokenType::INTEGER),
            Token("+", TokenType::OPERATOR),
            Token("6", TokenType::INTEGER),
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new NameSimpleSyntaxRule());
        list<Token> tokensInLHSRule = { Token("x", TokenType::NAME_OR_KEYWORD) };
        lhsRule->consumeTokens(tokensInLHSRule);

        // create operator rule
        shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
        list<Token> tokensInOperatorRule = { Token("%", TokenType::OPERATOR) };
        operatorRule->consumeTokens(tokensInOperatorRule);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ExpressionSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token("5", TokenType::INTEGER), Token("+", TokenType::OPERATOR), Token("6", TokenType::INTEGER) };
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
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
        };

        // create relational rule
        shared_ptr<SimpleSyntaxRule> relationalRule = shared_ptr<SimpleSyntaxRule>(new RelationalExpressionSimpleSyntaxRule());
        list<Token> tokensRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD),Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD), };
        relationalRule->consumeTokens(tokensRule);

        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
            relationalRule,
        };

        test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }    

    SECTION("ConditionalExpressionSimpleSyntaxRule: !(soomevariable >= x)") {
        list<Token> tokensToConsume = {
            Token(NOT, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER)
        };

        // create conditional rule
        shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD),
                                        Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
                                        Token("x", TokenType::NAME_OR_KEYWORD)};
        conRule->consumeTokens(tokensInRHSRule);

        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
            conRule
        };
        test(ConditionalExpressionSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }

    SECTION("ConditionalExpressionSimpleSyntaxRule: !(soomevariable >= x) && (soomevariable >= x)") {
        list<Token> tokensToConsume = {
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(AND, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER)
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
        list<Token> tokensInLHSRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD) };
        lhsRule->consumeTokens(tokensInLHSRule);

        // create operator rule
        shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
        list<Token> tokensInOperatorRule = { Token(AND, TokenType::OPERATOR) };
        operatorRule->consumeTokens(tokensInOperatorRule);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD) };
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
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(AND, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
        };

        // create lhs rule
        shared_ptr<SimpleSyntaxRule> lhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
        list<Token> tokensInLHSRule = { Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER) };
        lhsRule->consumeTokens(tokensInLHSRule);

        // create operator rule
        shared_ptr<SimpleSyntaxRule> operatorRule = shared_ptr<SimpleSyntaxRule>(new OperatorSimpleSyntaxRule());
        list<Token> tokensInOperatorRule = { Token(AND, TokenType::OPERATOR) };
        operatorRule->consumeTokens(tokensInOperatorRule);

        // create rhs rule
        shared_ptr<SimpleSyntaxRule> rhsRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
        list<Token> tokensInRHSRule = { Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("v", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OR, TokenType::OPERATOR),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("q", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER) };
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
            Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // create conditional rule
        shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
        list<Token> tokensInConRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD),
                                        Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
                                        Token("x", TokenType::NAME_OR_KEYWORD) };
        conRule->consumeTokens(tokensInConRule);

        // Create stmtList rule
        shared_ptr<SimpleSyntaxRule> stmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
        list<Token> tokensInStmtLstRule = { Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER) };
        stmtLstRule->consumeTokens(tokensInStmtLstRule);

        vector<shared_ptr<SimpleSyntaxRule>> expectedChildren = {
            conRule,
            stmtLstRule
        };
        test(WhileSimpleSyntaxRule(), tokensToConsume, expectedChildren);
    }

    SECTION("IfSimpleSyntaxRule: standard condition") {
        list<Token> tokensToConsume = {
            Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_BRACKET, TokenType::DELIMITER),
            Token("soomevariable", TokenType::NAME_OR_KEYWORD),
            Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_BRACKET, TokenType::DELIMITER),
            Token(THEN_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            Token(ELSE_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // create conditional rule
        shared_ptr<SimpleSyntaxRule> conRule = shared_ptr<SimpleSyntaxRule>(new ConditionalExpressionSimpleSyntaxRule());
        list<Token> tokensInConRule = { Token("soomevariable", TokenType::NAME_OR_KEYWORD),
                                        Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR),
                                        Token("x", TokenType::NAME_OR_KEYWORD) };
        conRule->consumeTokens(tokensInConRule);

        // Create stmtList rule
        shared_ptr<SimpleSyntaxRule> thenStmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
        list<Token> tokensInThenStmtLstRule = { Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER) };
        thenStmtLstRule->consumeTokens(tokensInThenStmtLstRule);

        shared_ptr<SimpleSyntaxRule> elseStmtLstRule = shared_ptr<SimpleSyntaxRule>(new StatementListSimpleSyntaxRule());
        list<Token> tokensInElseStmtLstRule = { Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD),
            Token("x", TokenType::NAME_OR_KEYWORD),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER) };
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
        Token constantValueToken = Token("1", TokenType::INTEGER);
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
        Token variableValueToken = Token("1", TokenType::NAME_OR_KEYWORD);
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
        Token readToken = Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token variable = Token("soomevariable", TokenType::NAME_OR_KEYWORD);
        Token semiColon = Token(SEMI_COLON, TokenType::DELIMITER);
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
        Token printToken = Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token variable = Token("soomevariable", TokenType::NAME_OR_KEYWORD);
        Token semiColon = Token(SEMI_COLON, TokenType::DELIMITER);
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
        Token leftHandSideToken = Token("x", TokenType::NAME_OR_KEYWORD);
        Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
        Token rightHandSideToken = Token("5", TokenType::INTEGER);
        Token semiColon = Token(SEMI_COLON, TokenType::DELIMITER);
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
        Token leftHandSideToken = Token("x", TokenType::NAME_OR_KEYWORD);
        Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
        Token rightXToken = Token("x", TokenType::NAME_OR_KEYWORD);
        Token plusToken = Token(PLUS_OPERATOR, TokenType::OPERATOR);
        Token rightOneToken = Token("1", TokenType::INTEGER);
        Token semiColon = Token(SEMI_COLON, TokenType::DELIMITER);
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
        Token leftHandSideToken = Token("x", TokenType::NAME_OR_KEYWORD);
        Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
        Token rightXToken = Token("x", TokenType::NAME_OR_KEYWORD);
        Token plusToken = Token(PLUS_OPERATOR, TokenType::OPERATOR);
        Token rightOneToken = Token("1", TokenType::INTEGER);
        Token semiColon = Token(SEMI_COLON, TokenType::DELIMITER);
        Token openBracket = Token(OPEN_BRACKET, TokenType::DELIMITER);
        Token closedBracket = Token(CLOSED_BRACKET, TokenType::DELIMITER);
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

        Token variable = Token("soomevariable", TokenType::NAME_OR_KEYWORD);
        Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
        Token constantToken = Token("1", TokenType::INTEGER);

        list<Token> tokensToConsume = {
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            variable,
            equalsToken,
            constantToken,
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // Create rule
        StatementListSimpleSyntaxRule rule = StatementListSimpleSyntaxRule();
        list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
        vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

        //Create StmtList node
        shared_ptr<ASTNode> expectedASTNode(new StatementListASTnode(Token{ "", TokenType::DELIMITER }));

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

        Token procedureToken = Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token procedureName = Token("procedureName", TokenType::NAME_OR_KEYWORD);
        Token variable = Token("soomevariable", TokenType::NAME_OR_KEYWORD);
        Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
        Token constantToken = Token("1", TokenType::INTEGER);

        list<Token> tokensToConsume = {
            procedureToken,
            procedureName,
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            variable,
            equalsToken,
            constantToken,
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // Create rule
        ProcedureSimpleSyntaxRule rule = ProcedureSimpleSyntaxRule();
        list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
        vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

        // Create procedure node
        shared_ptr<ASTNode> expectedASTNode(new ProcedureASTNode(procedureName));

        // Create stmtlst node
        shared_ptr<ASTNode> stmtLstASTNode(new StatementListASTnode(Token{ "", TokenType::DELIMITER }));

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

        Token procedureToken = Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token procedureName = Token("procedureName", TokenType::NAME_OR_KEYWORD);
        Token anotherProcedureName = Token("anotherProcedureName", TokenType::NAME_OR_KEYWORD);
        Token variable = Token("soomevariable", TokenType::NAME_OR_KEYWORD);
        Token anotherVariable = Token("variable", TokenType::NAME_OR_KEYWORD);
        Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
        Token constantToken = Token("1", TokenType::INTEGER);

        list<Token> tokensToConsume = {
            procedureToken,
            procedureName,
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            variable,
            equalsToken,
            constantToken,
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
            procedureToken,
            anotherProcedureName,
            Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
            anotherVariable,
            equalsToken,
            constantToken,
            Token(SEMI_COLON, TokenType::DELIMITER),
            Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
        };

        // Create rule
        ProgramSimpleSyntaxRule rule = ProgramSimpleSyntaxRule();
        list<Token> remainingTokens = rule.consumeTokens(tokensToConsume);
        vector<shared_ptr<SimpleSyntaxRule>> childRules = rule.generateChildRules();

        // Create expected ASTNode
        shared_ptr<ASTNode> expectedASTNode(new ProgramASTNode({ Token(PROGRAM_KEYWORD, TokenType::NAME_OR_KEYWORD) }));

        // Create first procedure node
        shared_ptr<ASTNode> firstProcedureNode(new ProcedureASTNode(procedureName));
        // Create second procedure node
        shared_ptr<ASTNode> secondProcedureNode(new ProcedureASTNode(anotherProcedureName));

        // Create firstStmtlst node
        shared_ptr<ASTNode> firstStmtLstASTNode(new StatementListASTnode(Token{ "", TokenType::DELIMITER }));

        // Create secondStmtlst node
        shared_ptr<ASTNode> secondStmtLstASTNode(new StatementListASTnode(Token{ "", TokenType::DELIMITER }));

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
        Token addToken = Token(PLUS_OPERATOR, TokenType::OPERATOR);
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
        Token xToken = Token("x", TokenType::NAME_OR_KEYWORD);
        Token modToken = Token("%", TokenType::OPERATOR);
        Token fiveToken = Token("5", TokenType::INTEGER);
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
        Token xToken = Token("x", TokenType::NAME_OR_KEYWORD);
        Token greaterToken = Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
        Token yToken = Token("y", TokenType::NAME_OR_KEYWORD);
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
        // (x > y) && (x > y);
        Token openBracket = Token(OPEN_BRACKET, TokenType::DELIMITER);
        Token variableX = Token("x", TokenType::NAME_OR_KEYWORD);
        Token greaterThan = Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
        Token variableY = Token("y", TokenType::NAME_OR_KEYWORD);
        Token closedBracket = Token(CLOSED_BRACKET, TokenType::DELIMITER);
        Token andToken = Token(AND, TokenType::OPERATOR);

        list<Token> tokensToConsume = { openBracket, variableX, greaterThan, variableY, closedBracket, 
                                        andToken,
                                        openBracket, variableX, greaterThan, variableY, closedBracket };

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
        Token whileToken = Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token variableX = Token("x", TokenType::NAME_OR_KEYWORD);
        Token greaterThan = Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
        Token variableY = Token("y", TokenType::NAME_OR_KEYWORD);
        Token readToken = Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token semiColonToken = Token(SEMI_COLON, TokenType::DELIMITER);

        list<Token> tokensToConsume = { Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD),
                                        Token(OPEN_BRACKET, TokenType::DELIMITER),
                                        variableX,
                                        greaterThan,
                                        variableY,
                                        Token(CLOSED_BRACKET, TokenType::DELIMITER),
                                        Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
                                        readToken,
                                        variableX,
                                        semiColonToken,
                                        Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER), };

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
        shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

        shared_ptr<ASTNode> readNode(new ReadASTNode(Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD)));
        shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

        readNode->addChild(xTwoNode);
        stmtLstNode->addChild(readNode);

        expectedASTNode->addChild(condNode);
        expectedASTNode->addChild(stmtLstNode);

        test(rule, expectedASTNode);
    }

    // -------------------- IfSimpleSyntaxRule --------------------
    SECTION("IfSimpleSyntaxRule : constructNode") {
        // while (x > y) then { read x; } else  { read y;}
        Token ifToken = Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token variableX = Token("x", TokenType::NAME_OR_KEYWORD);
        Token greaterThan = Token(GREATER_THAN_EQUAL_OPERATOR, TokenType::OPERATOR);
        Token variableY = Token("y", TokenType::NAME_OR_KEYWORD);
        Token readToken = Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD);
        Token semiColonToken = Token(SEMI_COLON, TokenType::DELIMITER);

        list<Token> tokensToConsume = { ifToken,
                                        Token(OPEN_BRACKET, TokenType::DELIMITER),
                                        variableX,
                                        greaterThan,
                                        variableY,
                                        Token(CLOSED_BRACKET, TokenType::DELIMITER),
                                        Token(THEN_KEYWORD, TokenType::NAME_OR_KEYWORD),
                                        Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
                                        readToken,
                                        variableX,
                                        semiColonToken,
                                        Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER),
                                        Token(ELSE_KEYWORD, TokenType::NAME_OR_KEYWORD),
                                        Token(OPEN_CURLY_BRACKET, TokenType::DELIMITER),
                                        readToken,
                                        variableY,
                                        semiColonToken,
                                        Token(CLOSED_CURLY_BRACKET, TokenType::DELIMITER), };

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
        shared_ptr<ASTNode> stmtLstOneNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

        shared_ptr<ASTNode> readOneNode(new ReadASTNode(Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD)));
        shared_ptr<ASTNode> xTwoNode(new VariableASTNode(variableX));

        // Create else stmtlst node
        shared_ptr<ASTNode> stmtLstTwoNode(new StatementListASTnode(Token("", TokenType::DELIMITER)));

        shared_ptr<ASTNode> readTwoNode(new ReadASTNode(Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD)));
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

    Token procedureToken = Token(PROCEDURE_KEYWORD, TokenType::NAME_OR_KEYWORD);
    Token procedureName = Token("procedureName", TokenType::NAME_OR_KEYWORD);
    Token variable = Token("soomevariable", TokenType::NAME_OR_KEYWORD);
    Token equalsToken = Token(EQUAL_OPERATOR, TokenType::OPERATOR);
    Token constantToken = Token("1", TokenType::INTEGER);

    // Create expected ASTNode
    shared_ptr<ASTNode> expectedASTNode(new ProgramASTNode({ Token(PROGRAM_KEYWORD, TokenType::NAME_OR_KEYWORD) }));

    // Create ASTNode with no line numbers
    shared_ptr<ASTNode> toSet(new ProgramASTNode({ Token(PROGRAM_KEYWORD, TokenType::NAME_OR_KEYWORD) }));

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
    shared_ptr<ASTNode> firstStmtLstASTNode(new StatementListASTnode(Token{ "", TokenType::DELIMITER }));

    // Create firstStmtlst node w/o line numbers
    shared_ptr<ASTNode> toSetStmtLstASTNode(new StatementListASTnode(Token{ "", TokenType::DELIMITER }));

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

// TODO
TEST_CASE("ProgramSimpleSyntaxRule test ::handleStatementList") {

}

// TODO
TEST_CASE("ProgramSimpleSyntaxRule test ::recursiveSetLineNumber") {

}

// TODO
TEST_CASE("ProgramSimpleSyntaxRule test ::recursiveSetStatementNumber") {

}


// =============== INTEGRATION TEST FOR RULES ====================