#include "catch.hpp"
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <vector>

using namespace std;

TEST_CASE("AST: Add Child") {
	auto testAddChild = [](shared_ptr<ASTNode> nodeToAdd, shared_ptr<ASTNode> nodeToAddTo) {
		int prevSize = nodeToAddTo->numChildren();

		if (nodeToAddTo->isTerminal()) {
			REQUIRE_THROWS(nodeToAddTo->addChild(nodeToAdd));
		}
		else {
			nodeToAddTo->addChild(nodeToAdd);
			REQUIRE(nodeToAddTo->numChildren() == prevSize + 1);
		}
	};

	SECTION("AssignASTNode") {
		shared_ptr<ASTNode> toAdd = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> toAddTo = AssignASTNode::createAssignNode();
		testAddChild(toAdd, toAddTo);
	}
	SECTION("CallASTNode") {
		shared_ptr<ASTNode> toAddTo = CallASTNode::createCallNode();
		shared_ptr<ASTNode> toAdd = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ConstantValueASTNode") {
		shared_ptr<ASTNode> toAdd = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("1"));
		shared_ptr<ASTNode> toAddTo = ConstantValueASTNode::createConstantNode(Token::createIntegerToken("1"));
		// Should throw an error
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ExpressionASTNode") {
		shared_ptr<ASTNode> toAdd = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> toAddTo = ExpressionASTNode::createExpressionNode(Token::createLessThanToken());
		testAddChild(toAdd, toAddTo);
	}
	SECTION("IfASTNode") {
		shared_ptr<ASTNode> toAdd = ExpressionASTNode::createExpressionNode(Token::createGreaterThanToken());
		shared_ptr<ASTNode> toAddTo = IfASTNode::createIfNode();
		testAddChild(toAdd, toAddTo);
	}
	SECTION("PrintASTNode") {
		shared_ptr<ASTNode> toAdd = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> toAddTo = PrintASTNode::createPrintNode();		
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ProcedureASTNode") {
		shared_ptr<ASTNode> toAddTo = ProcedureASTNode::createProcedureNode(Token::createNameOrKeywordToken("main"));
		shared_ptr<ASTNode> toAdd = StatementListASTNode::createStatementListNode();		
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ProgramASTNode") {
		shared_ptr<ASTNode> toAddTo = ProgramASTNode::createProgramNode();		
		shared_ptr<ASTNode> toAdd = ProcedureASTNode::createProcedureNode(Token::createNameOrKeywordToken("main"));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ReadASTNode") {
		shared_ptr<ASTNode> toAdd = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> toAddTo = ReadASTNode::createReadNode();		
		testAddChild(toAdd, toAddTo);
	}
	SECTION("StatementListASTNode") {
		shared_ptr<ASTNode> toAddTo = StatementListASTNode::createStatementListNode();		
		shared_ptr<ASTNode> toAdd = AssignASTNode::createAssignNode();
		testAddChild(toAdd, toAddTo);
	}
	SECTION("VariableASTNode") {
		shared_ptr<ASTNode> toAddTo = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		shared_ptr<ASTNode> toAdd = VariableASTNode::createVariableNode(Token::createNameOrKeywordToken("x"));
		// Should throw an error
		testAddChild(toAdd, toAddTo);
	}
	SECTION("WhileASTNode") {
		shared_ptr<ASTNode> toAddTo = WhileASTNode::createWhileNode();		
		shared_ptr<ASTNode> toAdd = ExpressionASTNode::createExpressionNode(Token::createGreaterThanToken());
		testAddChild(toAdd, toAddTo);
	}
}

TEST_CASE("AST: Change line number") {
	auto testChangeLineNumber = [](int lineNumber) {
		shared_ptr<ASTNode> toChangeLineNumber = AssignASTNode::createAssignNode();

		// Ensure intial linenumber is -1
		REQUIRE(toChangeLineNumber->getLineNumber() == -1);

		toChangeLineNumber->setLineNumber(lineNumber);

		REQUIRE(toChangeLineNumber->getLineNumber() == lineNumber);
	};

	testChangeLineNumber(5);
	testChangeLineNumber(2);
	testChangeLineNumber(0);
}