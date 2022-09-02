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
#include <sp/dataclasses/tokens/Token.h>
#include <list>
#include <memory>
using namespace std;

// =============== UNIT TEST FOR RULES ====================

TEST_CASE("Parser: test consumeTokens") {

    auto test = [](SimpleSyntaxRule &rule, list<Token> tokens, list<Token> expectedTokens) {
        
        // ----- when -----
        list<Token> remainingTokens = rule.consumeTokens(tokens);

        // ----- then -----
        REQUIRE(remainingTokens.size() == expectedTokens.size()); // same size
        while (!remainingTokens.empty()) {
            Token t1 = remainingTokens.front();
            Token t2 = expectedTokens.front();
            REQUIRE(t1.equals(t2));
            remainingTokens.pop_front();
            expectedTokens.pop_front();
            
        }
    };

    auto testThrowsException = [](SimpleSyntaxRule& rule, list<Token> tokens) {

        // ----- when & then -----
        REQUIRE_THROWS(rule.consumeTokens(tokens));
    };

    // run tests

    // -------------------- ConstantValueSimpleSyntaxRule --------------------
    SECTION("ConstValueSimpleSyntaxRule: Exactly one token left, numeric") {
        list<Token> tokens = { Token("1", TokenType::INTEGER) };
        list<Token> expectedTokens = {};
        test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
    }
    SECTION("ConstValueSimpleSyntaxRule: Exactly one token left, very large number") {
        list<Token> tokens = { Token("12345435", TokenType::INTEGER) };
        list<Token> expectedTokens = {};
        test(ConstantValueSimpleSyntaxRule(), tokens, expectedTokens);
    }

    SECTION("ConstValueSimpleSyntaxRule: Exactly one token left, not purely numeric, throws") {
        list<Token> tokens = { Token(";", TokenType::DELIMITER) };
        testThrowsException(ConstantValueSimpleSyntaxRule(), tokens);
    }
    SECTION("ConstValueSimpleSyntaxRule: Multiple tokens, throws") {
        list<Token> tokens = { Token("12345435", TokenType::INTEGER), Token("12345435", TokenType::INTEGER) };
        testThrowsException(ConstantValueSimpleSyntaxRule(), tokens);
    }

    // -------------------- NameSimpleSyntaxRule --------------------


}

TEST_CASE("Parser: test ConstantValueSimpleSyntaxRule::generateChildRules") {

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
    

}
