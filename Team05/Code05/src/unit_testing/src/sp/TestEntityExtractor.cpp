#include "catch.hpp"
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/EntityExtractor.cpp>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.cpp>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.cpp>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.cpp>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/CallASTNode.cpp>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.cpp>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.cpp>
#include <sp/dataclasses/ast/VariableASTnode.h>
#include <sp/dataclasses/ast/VariableASTnode.cpp>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.cpp>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>
#include <memory>

using namespace std;

TEST_CASE("EntityExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Entity> expectedEntityVector) {
		// Given
		EntityExtractor extractor = EntityExtractor();

		// When
		vector<Entity> extractedEntity = extractor.extract(nodeToExtractFrom);

		// Tehn
		REQUIRE(extractedEntity.size() == expectedEntityVector.size());

		for (int i = 0; i < extractedEntity.size(); i++) {
			REQUIRE(extractedEntity[i].equals(expectedEntityVector[i]));
		}
	};

	SECTION("procedure main") {
		Token procedureNameToken = Token{ "main", TokenType::NAME_OR_KEYWORD };

		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(procedureNameToken));
		procedureNode->setLineNumber(1);

		vector<Entity> expectedEntity{ Entity{EntityType::PROCEDURE, 1, procedureNameToken} };

		testExtract(procedureNode, expectedEntity);
	}

	SECTION("read x") {
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token readToken = Token{ READ_KEYWORD, TokenType::NAME_OR_KEYWORD };

		shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
		readNode->setLineNumber(1);

		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		xNode->setLineNumber(1);

		readNode->addChild(xNode);

		vector<Entity> expectedEntity = vector<Entity>{ Entity(EntityType::READ, 1, readToken), Entity(EntityType::VARIABLE, 1, xToken) };

		testExtract(readNode, expectedEntity);
	}
	SECTION("print y") {
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
		Token printToken = Token{ PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD };

		shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));
		printNode->setLineNumber(1);

		shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));
		yNode->setLineNumber(1);

		printNode->addChild(yNode);

		vector<Entity> expectedEntity{ Entity{EntityType::PRINT, 1, printToken}, Entity{EntityType::VARIABLE, 1, yToken } };

		testExtract(printNode, expectedEntity);
	}
	SECTION("x = 1") {
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
		Token constantToken = Token{ "1", TokenType::INTEGER };


		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		assignNode->setLineNumber(1);

		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		xNode->setLineNumber(1);

		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));
		constantNode->setLineNumber(1);

		assignNode->addChild(xNode);
		assignNode->addChild(constantNode);


		vector<Entity> expectedEntity{ Entity{EntityType::ASSIGN, 1, assignToken}, Entity{EntityType::VARIABLE, 1, xToken} };

		testExtract(assignNode, expectedEntity);

	}
	SECTION("call somefunction") {
		Token callNameToken = Token{ "somefunction", TokenType::NAME_OR_KEYWORD };

		shared_ptr<ASTNode> callNode(new CallASTNode(callNameToken));
		callNode->setLineNumber(1);

		vector<Entity> expectedEntity{ Entity{EntityType::CALL, 1, callNameToken} };

		testExtract(callNode, expectedEntity);
	}

	// Not yet implemented
	SECTION("while") {}
	// Not yet implemented
	SECTION("if") {}
}