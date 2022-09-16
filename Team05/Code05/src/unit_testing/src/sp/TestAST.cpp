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
		shared_ptr<ASTNode> toAdd(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> toAddTo(new AssignASTNode(Token::createEqualsToken()));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("CallASTNode") {
		shared_ptr<ASTNode> toAddTo(new CallASTNode (Token::createCallToken()));
		shared_ptr<ASTNode> toAdd(new VariableASTNode (Token::createNameOrKeywordToken("x")));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ConstantValueASTNode") {
		shared_ptr<ASTNode> toAdd(new ConstantValueASTNode(Token::createIntegerToken("1")));
		shared_ptr<ASTNode> toAddTo(new ConstantValueASTNode(Token::createIntegerToken("1")));
		// Should throw an error
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ExpressionASTNode") {
		shared_ptr<ASTNode> toAdd(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> toAddTo(new ExpressionASTNode(Token::createLessThanToken()));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("IfASTNode") {
		shared_ptr<ASTNode> toAdd(new ExpressionASTNode(Token::createGreaterThanToken()));
		shared_ptr<ASTNode> toAddTo(new IfASTNode(Token::createIfToken()));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("PrintASTNode") {
		shared_ptr<ASTNode> toAdd(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> toAddTo(new PrintASTNode(Token::createPrintToken()));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ProcedureASTNode") {
		shared_ptr<ASTNode> toAddTo(new ProcedureASTNode(Token::createNameOrKeywordToken("main")));
		shared_ptr<ASTNode> toAdd(new StatementListASTNode(Token::createPlaceholderToken()));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ProgramASTNode") {
		shared_ptr<ASTNode> toAddTo(new ProgramASTNode(Token::createProgramToken()));
		shared_ptr<ASTNode> toAdd(new ProcedureASTNode(Token::createNameOrKeywordToken("main")));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("ReadASTNode") {
		shared_ptr<ASTNode> toAdd(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> toAddTo(new ReadASTNode(Token::createReadToken()));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("StatementListASTNode") {
		shared_ptr<ASTNode> toAddTo(new StatementListASTNode(Token::createPlaceholderToken()));
		shared_ptr<ASTNode> toAdd(new AssignASTNode(Token::createEqualsToken()));
		testAddChild(toAdd, toAddTo);
	}
	SECTION("VariableASTNode") {
		shared_ptr<ASTNode> toAddTo(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		shared_ptr<ASTNode> toAdd(new VariableASTNode(Token::createNameOrKeywordToken("x")));
		// Should throw an error
		testAddChild(toAdd, toAddTo);
	}
	SECTION("WhileASTNode") {
		shared_ptr<ASTNode> toAddTo(new WhileASTNode(Token::createWhileToken()));
		shared_ptr<ASTNode> toAdd(new ExpressionASTNode(Token::createGreaterThanToken()));
		testAddChild(toAdd, toAddTo);
	}
}

TEST_CASE("AST: Change line number") {
	auto testChangeLineNumber = [](int lineNumber) {
		shared_ptr<ASTNode> toChangeLineNumber(new AssignASTNode(Token::createNameOrKeywordToken("x")));

		// Ensure intial linenumber is -1
		REQUIRE(toChangeLineNumber->getLineNumber() == -1);

		toChangeLineNumber->setLineNumber(lineNumber);

		REQUIRE(toChangeLineNumber->getLineNumber() == lineNumber);
	};

	testChangeLineNumber(5);
	testChangeLineNumber(2);
	testChangeLineNumber(0);
}