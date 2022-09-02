#include "catch.hpp"

#include <sp/dataclasses/AST.cpp>
#include <sp/dataclasses/AST.h>
#include <sp/parser/rules/ProgramSimpleSyntaxRule.cpp>
#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>
#include <sp/parser/rules/AssignSimpleSyntaxRule.cpp>
#include <sp/parser/rules/AssignSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.h>
#include <sp/parser/rules/ConstantValueSimpleSyntaxRule.cpp>
#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/rules/ProcedureSimpleSyntaxRule.cpp>
#include <sp/parser/rules/ReadSimpleSyntaxRule.h>
#include <sp/parser/rules/ReadSimpleSyntaxRule.cpp>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.cpp>
#include <sp/parser/rules/NameSimpleSyntaxRule.h>
#include <sp/parser/rules/NameSimpleSyntaxRule.cpp>
#include <sp/parser/rules/StatementListSimpleSyntaxRule.h>
#include <sp/parser/rules/StatementListSimpleSyntaxRule.cpp>

#include <sp/dataclasses/tokens/Token.h>
#include <list>
#include <memory>
using namespace std;

// =============== UNIT TEST FOR RULES ====================

TEST_CASE("Parser: test ::consumeTokens") {

    auto test = [](SimpleSyntaxRule &rule, list<Token> tokens, list<Token> expectedTokens) {
        
        // ----- when -----
        list<Token> remainingTokens = rule.consumeTokens(tokens);

        // ----- then -----
        bool sameSize = remainingTokens.size() == expectedTokens.size();
        REQUIRE(sameSize); // same size
        if (sameSize) {
            while (!remainingTokens.empty()) {
                Token t1 = remainingTokens.front();
                Token t2 = expectedTokens.front();
                REQUIRE(t1.equals(t2));
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

    // -------------------- ReadSimpleSyntaxRule --------------------
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
    SECTION("AssignSimpleSyntaxRule: nested is the lhs and rhs, should have name lhs and expression rhs generated") {
        // TODO
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
}


// =============== INTEGRATION TEST FOR RULES ====================