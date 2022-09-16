#include "catch.hpp"
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/BracketsASTNode.h>
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
		Token procedureNameToken = Token::createNameOrKeywordToken("main");

		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(procedureNameToken));
		procedureNode->setLineNumber(1);

		vector<Entity> expectedEntity{ Entity::createProcedureEntity(procedureNameToken)};

		testExtract(procedureNode, expectedEntity);
	}

	SECTION("read x") {
		Token xToken = Token::createNameOrKeywordToken("x");
		Token readToken = Token::createReadToken();

		shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
		readNode->setLineNumber(1);

		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		xNode->setLineNumber(1);

		readNode->addChild(xNode);

		vector<Entity> expectedEntity = vector<Entity>{ Entity::createReadEntity(1), Entity::createVariableEntity(1, xToken) };

		testExtract(readNode, expectedEntity);
	}
	SECTION("print y") {
		Token yToken = Token::createNameOrKeywordToken("y");
		Token printToken = Token::createPrintToken();

		shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));
		printNode->setLineNumber(1);

		shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));
		yNode->setLineNumber(1);

		printNode->addChild(yNode);

		vector<Entity> expectedEntity{ Entity::createPrintEntity(1), Entity::createVariableEntity(1, yToken)};

		testExtract(printNode, expectedEntity);
	}
	SECTION("x = 1") {
		Token xToken = Token::createNameOrKeywordToken("x");
		Token assignToken = Token::createEqualsToken();
		Token constantToken = Token::createIntegerToken("1");


		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		assignNode->setLineNumber(1);

		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		xNode->setLineNumber(1);

		shared_ptr<ASTNode> constantNode(new ConstantValueASTNode(constantToken));
		constantNode->setLineNumber(1);

		assignNode->addChild(xNode);
		assignNode->addChild(constantNode);


		vector<Entity> expectedEntity{ Entity::createAssignEntity(1), Entity::createVariableEntity(1, xToken), Entity::createConstantEntity(1, constantToken)};

		testExtract(assignNode, expectedEntity);

	}
	SECTION("call somefunction") {
		Token callNameToken = Token::createNameOrKeywordToken("somefunction");

		shared_ptr<ASTNode> callNode(new CallASTNode(callNameToken));
		callNode->setLineNumber(1);

		vector<Entity> expectedEntity{ Entity::createCallEntity(1)};

		testExtract(callNode, expectedEntity);
	}

	// Not yet implemented
	SECTION("while") {}
	// Not yet implemented
	SECTION("if") {}
}