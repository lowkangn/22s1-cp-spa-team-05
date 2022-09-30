#include "catch.hpp"

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>


#include <sp/dataclasses/tokens/Token.h>
#include <list>
#include <memory>

#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>


TEST_CASE("ProgramSimpleSyntaxRule::consumeTokens") {

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
}

TEST_CASE("ProgramSimpleSyntaxRule::generateChildRules") {

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

}

TEST_CASE("ProgramSimpleSyntaxRule::constructNode") {
	auto test = [](SimpleSyntaxRule& rule, shared_ptr<ASTNode> expectedASTNode) {

		shared_ptr<ASTNode> generatedNode = rule.constructNode();

		REQUIRE(generatedNode->equals(expectedASTNode));
	};

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
		shared_ptr<ASTNode> expectedASTNode = ProgramASTNode::createProgramNode();
		// Create first procedure node
		shared_ptr<ASTNode> firstProcedureNode = ProcedureASTNode::createProcedureNode(procedureName);
		// Create second procedure node
		shared_ptr<ASTNode> secondProcedureNode = ProcedureASTNode::createProcedureNode(anotherProcedureName);

		// Create firstStmtlst node
		shared_ptr<ASTNode> firstStmtLstASTNode = StatementListASTNode::createStatementListNode();
		// Create secondStmtlst node
		shared_ptr<ASTNode> secondStmtLstASTNode = StatementListASTNode::createStatementListNode();
		// Create first assign node
		shared_ptr<ASTNode> firstAssignASTNode = AssignASTNode::createAssignNode();

		// Create first assign node
		shared_ptr<ASTNode> secondAssignASTNode = AssignASTNode::createAssignNode();


		// Create first variable node
		shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(variable);


		// Create second variable node
		shared_ptr<ASTNode> anotherVariableNode = VariableASTNode::createVariableNode(anotherVariable);


		// Create constant node
		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constantToken);


		// Create another constant node
		shared_ptr<ASTNode> anotherConstantNode = ConstantValueASTNode::createConstantNode(constantToken);


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
}

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
	shared_ptr<ASTNode> expectedASTNode = ProgramASTNode::createProgramNode();
	// Create ASTNode with no line numbers
	shared_ptr<ASTNode> toSet = ProgramASTNode::createProgramNode();
	// Create first procedure node
	shared_ptr<ASTNode> firstProcedureNode = ProcedureASTNode::createProcedureNode(procedureName);

	// Create first procedure node w/o line numbers
	shared_ptr<ASTNode> toSetProcedureNode = ProcedureASTNode::createProcedureNode(procedureName);

	// Create first variable node
	shared_ptr<ASTNode> variableNode = VariableASTNode::createVariableNode(variable);
	variableNode->setLineNumber(1);

	// Create first variable node w/o Line numbers
	shared_ptr<ASTNode> toSetVariableNode = VariableASTNode::createVariableNode(variable);

	// Create constant node
	shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constantToken);
	constantNode->setLineNumber(1);

	// Create constant node w/o line numbers
	shared_ptr<ASTNode> toSetConstantNode = ConstantValueASTNode::createConstantNode(constantToken);

	// Create firstStmtlst node
	shared_ptr<ASTNode> firstStmtLstASTNode = StatementListASTNode::createStatementListNode();
	// Create firstStmtlst node w/o line numbers
	shared_ptr<ASTNode> toSetStmtLstASTNode = StatementListASTNode::createStatementListNode();
	// Create first assign node
	shared_ptr<ASTNode> firstAssignASTNode = AssignASTNode::createAssignNode();
	firstAssignASTNode->setLineNumber(1);

	// Create first assign node w/o line numbers
	shared_ptr<ASTNode> toSetAssignASTNode = AssignASTNode::createAssignNode();

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