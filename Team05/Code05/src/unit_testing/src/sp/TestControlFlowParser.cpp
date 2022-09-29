#include "catch.hpp"

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>
#include <sp/parser/cfg_parser/ControlFlowGraphparser.h>

#include <vector>
#include <memory>

using namespace std;

TEST_CASE("addChildToTheEndOfRoot test") {
	auto test = [](shared_ptr<CFGNode> toAdd, shared_ptr<CFGNode> toAddTo, shared_ptr<CFGNode> expected) {
		ControlFlowParser cfgParser = ControlFlowParser();

		cfgParser.addChildToTheEndOfRoot(toAddTo, toAdd);

		REQUIRE(toAddTo->equals(expected));
	};

	SECTION("Simple test") {
		shared_ptr<CFGNode> root(new CFGNode(1));
		shared_ptr<CFGNode> child(new CFGNode(2));

		shared_ptr<CFGNode> expected(new CFGNode(1));
		shared_ptr<CFGNode> expectedChild(new CFGNode(2));

		expected->addNext(expectedChild);

		test(child, root, expected);
	}

	SECTION("Traverse till end test") {
		shared_ptr<CFGNode> root(new CFGNode(1));
		shared_ptr<CFGNode> rootChild(new CFGNode(2));
		root->addNext(rootChild);

		shared_ptr<CFGNode> childToAdd(new CFGNode(3));

		shared_ptr<CFGNode> expected(new CFGNode(1));
		shared_ptr<CFGNode> expectedRootChild(new CFGNode(2));
		shared_ptr<CFGNode> expectedChild(new CFGNode(3));

		expected->addNext(expectedRootChild);
		expectedRootChild->addNext(expectedChild);

		test(childToAdd, root, expected);
	}

	SECTION("Test if") {
		shared_ptr<CFGNode> root(new IfCFGNode(1));

		shared_ptr<CFGNode> thenRoot(new CFGNode(2));
		shared_ptr<CFGNode> thenStatement(new CFGNode(3));
		thenRoot->addNext(thenStatement);

		shared_ptr<CFGNode> elseRoot(new CFGNode(4));
		shared_ptr<CFGNode> elseStatement(new CFGNode(5));
		elseRoot->addNext(elseStatement);

		shared_ptr<CFGNode> childToAdd(new CFGNode(6));

		root->addNext(thenRoot);
		root->addNext(elseRoot);

		shared_ptr<CFGNode> expected(new IfCFGNode(1));
		
		shared_ptr<CFGNode> expectedThenRoot(new CFGNode(2));
		shared_ptr<CFGNode> expectedThenStatement(new CFGNode(3));
		expectedThenRoot->addNext(expectedThenStatement);

		shared_ptr<CFGNode> expectedElseRoot(new CFGNode(4));
		shared_ptr<CFGNode> expectedElseStatement(new CFGNode(5));
		expectedElseRoot->addNext(expectedElseStatement);

		shared_ptr<CFGNode> expectedChild(new CFGNode(6));

		expectedThenStatement->addNext(expectedChild);
		expectedElseStatement->addNext(expectedChild);

		expected->addNext(expectedThenRoot);
		expected->addNext(expectedElseRoot);

		test(childToAdd, root, expected);
	}

	SECTION("Test while with statements") {
		shared_ptr<CFGNode> root(new WhileCFGNode(1));

		shared_ptr<CFGNode> whileStmtOne(new CFGNode(2));
		shared_ptr<CFGNode> whileStmtTwo(new CFGNode(3));
		root->addNext(whileStmtOne);
		whileStmtOne->addNext(whileStmtTwo);
		whileStmtTwo->addNext(root);

		shared_ptr<CFGNode> childToAdd(new CFGNode(4));

		shared_ptr<CFGNode> expectedRoot(new WhileCFGNode(1));

		shared_ptr<CFGNode> expectedWhileStmtOne(new CFGNode(2));
		shared_ptr<CFGNode> expectedWhileStmtTwo(new CFGNode(3));
		expectedRoot->addNext(expectedWhileStmtOne);
		expectedWhileStmtOne->addNext(expectedWhileStmtTwo);
		expectedWhileStmtTwo->addNext(expectedRoot);

		shared_ptr<CFGNode> expectedChild(new CFGNode(4));

		expectedRoot->addNext(expectedChild);

		test(childToAdd, root, expectedRoot);
	}
}

TEST_CASE("handleStatementList test") {
	auto test = [](shared_ptr<ASTNode> statementListASTNode, shared_ptr<CFGNode> expectedCFGNode) {
		ControlFlowParser cfgParser = ControlFlowParser();

		shared_ptr<CFGNode> generatedCFGNode = cfgParser.handleStatementList(statementListASTNode);

		REQUIRE(generatedCFGNode->equals(expectedCFGNode));
	};

	SECTION("Statement only test") {

	}

	SECTION("Statement with if test") {

	}

	SECTION("Statement with while test") {

	}

	// Then and else must point to start of while
	SECTION("Nested if inside while test") {

	}
}

TEST_CASE("handleIf test") {
	auto test = [](shared_ptr<ASTNode> ifASTNode, shared_ptr<CFGNode> expectedCFGNode) {
		ControlFlowParser cfgParser = ControlFlowParser();

		shared_ptr<CFGNode> generatedCFGNode = cfgParser.handleIf(ifASTNode);

		REQUIRE(generatedCFGNode->equals(expectedCFGNode));
	};

	SECTION("If with one statement in then and else") {

	}

	// Test diamond
	SECTION("If with one statement in then and else and one statement after") {

	}

	SECTION("Nested if") {

	}
}

TEST_CASE("handleWhile test") {
	auto test = [](shared_ptr<ASTNode> whileASTNode, shared_ptr<CFGNode> expectedCFGNode) {
		ControlFlowParser cfgParser = ControlFlowParser();

		shared_ptr<CFGNode> generatedCFGNode = cfgParser.handleWhile(whileASTNode);

		REQUIRE(generatedCFGNode->equals(expectedCFGNode));
	};

	// Test recursion
	SECTION("While with one statement in loop") {

	}

	// Full test
	SECTION("While with one statment in loop and one after") {

	}

	SECTION("Nested while") {

	}
}

TEST_CASE("handleProcedure test") {

}
