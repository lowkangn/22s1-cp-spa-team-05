#include "catch.hpp"
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <vector>

using namespace std;

TEST_CASE("AST: Add Child") {
	auto testAddChild = [](shared_ptr<ASTNode> nodeToAdd) {
		shared_ptr<ASTNode> toAddTo(new AssignASTNode(Token("x", TokenType::NAME_OR_KEYWORD)));
		int prevSize = toAddTo->numChildren();

		toAddTo->addChild(nodeToAdd);

		REQUIRE(toAddTo->numChildren() == prevSize + 1);
	};

	shared_ptr<ASTNode> toAdd(new AssignASTNode(Token("x", TokenType::NAME_OR_KEYWORD)));
	testAddChild(toAdd);
}

TEST_CASE("AST: Change line number") {
	auto testChangeLineNumber = [](int lineNumber) {
		shared_ptr<ASTNode> toChangeLineNumber(new AssignASTNode(Token("x", TokenType::NAME_OR_KEYWORD)));

		// Ensure intial linenumber is -1
		REQUIRE(toChangeLineNumber->getLineNumber() == -1);

		toChangeLineNumber->setLineNumber(lineNumber);

		REQUIRE(toChangeLineNumber->getLineNumber() == lineNumber);
	};

	testChangeLineNumber(5);
	testChangeLineNumber(2);
	testChangeLineNumber(0);
}