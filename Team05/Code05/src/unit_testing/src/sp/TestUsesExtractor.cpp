#include "catch.hpp"
#include <sp/design_extractor/UsesExtractor.h>
#include <sp/design_extractor/UsesExtractor.cpp>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>
#include <memory>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>

using namespace std;

TEST_CASE("UsesExtractor: test handleAssign") {


	auto handleAssign = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		UsesExtractor extractor = UsesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleAssign(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	
	// x = x + 1
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };

	Entity assignEntity = Entity(EntityType::ASSIGN, 1, assignToken);

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> addNode(new ExpressionASTNode(addToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	Entity xEntity = Entity{ EntityType::VARIABLE, 1, xToken };

	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
	Entity constEntity = Entity{ EntityType::CONSTANT, 1, constToken };


	addNode->setLineNumber(1);
	assignNode->setLineNumber(1);
	x->setLineNumber(1);
	constNode->setLineNumber(1);

	assignNode->addChild(x);
	assignNode->addChild(addNode);

	addNode->addChild(x);
	addNode->addChild(constNode);


	Relationship usesX = Relationship{ assignEntity, xEntity, RelationshipType::USES };

	vector<Relationship> expectedResult = vector<Relationship>{ usesX };

	handleAssign(assignNode, expectedResult);
}