#include "catch.hpp"
#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <vector>

using namespace std;

TEST_CASE("AST: Add Child") {
	auto testAddChild = [](ASTNode nodeToAdd) {
		ASTNode* toAddTo = new ASTNode(vector<Token>());
		int prevSize = toAddTo->numChildren();

		toAddTo->addChild(nodeToAdd);

		REQUIRE(toAddTo->numChildren() == prevSize + 1);
	};

	ASTNode* toAdd = new ASTNode(vector<Token>());
	testAddChild(*toAdd);
}

TEST_CASE("AST: Change line number") {
	auto testChangeLineNumber = [](int lineNumber) {
		ASTNode* toChangeLineNumber = new ASTNode(vector<Token>());

		// Ensure intial linenumber is -1
		REQUIRE(toChangeLineNumber->getLineNumber() == -1);

		toChangeLineNumber->changeLineNumber(lineNumber);

		REQUIRE(toChangeLineNumber->getLineNumber() == lineNumber);
	};

	testChangeLineNumber(5);
	testChangeLineNumber(2);
	testChangeLineNumber(0);
}