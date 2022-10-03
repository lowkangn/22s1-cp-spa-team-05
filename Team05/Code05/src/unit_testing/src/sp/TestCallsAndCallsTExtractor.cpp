#include "catch.hpp"

#include <sp/design_extractor/CallsAndCallsTExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>	
#include <memory>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>

using namespace std;

TEST_CASE("CallsAndCallsTExtractor: test extract") {
	auto test = [](shared_ptr<ASTNode> astNode, vector<Relationship> expectedResult) {
		// Given
		CallsAndCallsTExtractor extractor = CallsAndCallsTExtractor();

		// When
		vector<Relationship> extractedRelationships = extractor.extract(astNode);

		// Then
		REQUIRE(extractedRelationships.size() == expectedResult.size());

		for (int i = 0; i < extractedRelationships.size(); i++)
		{
			REQUIRE(extractedRelationships[i].equals(expectedResult[i]));
		}
	};

	// Variables are omitted for brevity. CallsAndCallsTExtractor does not interact with variables.
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token alphaToken = Token::createNameOrKeywordToken("alpha");
	Token betaToken = Token::createNameOrKeywordToken("beta");
	Token callToken = Token::createCallToken();

	SECTION("Test simple program - empty results") {
		
		/*
			procedure main {}

			procedure alpha {}

			procedure beta {}
		*/
		
		// procedure main
		shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
		mainProcedureNode->addChild(mainStmtListNode);

		// procedure alpha
		shared_ptr<ASTNode> alphaProcedureNode = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> alphaStmtListNode = StatementListASTNode::createStatementListNode();
		alphaProcedureNode->addChild(alphaStmtListNode);

		// procedure beta
		shared_ptr<ASTNode> betaProcedureNode = ProcedureASTNode::createProcedureNode(betaToken);
		shared_ptr<ASTNode> betaStmtListNode = StatementListASTNode::createStatementListNode();
		betaProcedureNode->addChild(betaStmtListNode);
		
		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);
		programNode->addChild(betaProcedureNode);

		vector<Relationship> expectedCallsRelationships{};

		test(programNode, expectedCallsRelationships);
	}

	SECTION("Test simple program - non-empty results") {

		/*
			procedure main {
				call alpha;
				call beta;
			}

			procedure alpha {
				call beta;
			}

			procedure beta {}
		*/

		// procedure main
		shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
		mainProcedureNode->addChild(mainStmtListNode);

		// procedure alpha
		shared_ptr<ASTNode> alphaProcedureNode = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> alphaStmtListNode = StatementListASTNode::createStatementListNode();
		alphaProcedureNode->addChild(alphaStmtListNode);

		// procedure beta
		shared_ptr<ASTNode> betaProcedureNode = ProcedureASTNode::createProcedureNode(betaToken);
		shared_ptr<ASTNode> betaStmtListNode = StatementListASTNode::createStatementListNode();
		betaProcedureNode->addChild(betaStmtListNode);

		shared_ptr<ASTNode> callAlphaNode = CallASTNode::createCallNode();
		callAlphaNode->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callAlphaNode);

		shared_ptr<ASTNode> callMainBetaNode = CallASTNode::createCallNode();
		callMainBetaNode->addChild(betaProcedureNode);
		mainStmtListNode->addChild(callMainBetaNode);

		shared_ptr<ASTNode> callAlphaBetaNode = CallASTNode::createCallNode();
		callAlphaBetaNode->addChild(betaProcedureNode);
		alphaStmtListNode->addChild(callAlphaBetaNode);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);
		programNode->addChild(betaProcedureNode);

		// Create entities
		Entity mainEntity = Entity::createProcedureEntity(mainToken);
		Entity alphaEntity = Entity::createProcedureEntity(alphaToken);
		Entity betaEntity = Entity::createProcedureEntity(betaToken);

		// Create relationships
		Relationship callsMainAlpha = Relationship::createCallsRelationship(mainEntity, alphaEntity);
		Relationship callsMainBeta = Relationship::createCallsRelationship(mainEntity, betaEntity);
		Relationship callsAlphaBeta = Relationship::createCallsRelationship(alphaEntity, betaEntity);

		vector<Relationship> expectedCallsRelationships{ callsMainAlpha, callsMainBeta, callsAlphaBeta };

		test(programNode, expectedCallsRelationships);
	}	
}

TEST_CASE("CallsAndCallsTExtractor: test extractWithSemanticError") {
	auto testExtractWithError = [](shared_ptr<ASTNode> astNode, string expectedErrorMessage) {
			// given
			CallsAndCallsTExtractor extractor = CallsAndCallsTExtractor();

			// then
			REQUIRE_THROWS_MATCHES(extractor.extract(astNode), ASTException, Catch::Message(expectedErrorMessage));
	};

	// Variables are omitted for brevity. CallsAndCallsTExtractor does not interact with variables.
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token alphaToken = Token::createNameOrKeywordToken("alpha");
	Token betaToken = Token::createNameOrKeywordToken("beta");
	Token callToken = Token::createCallToken();

	SECTION("Procedure calling itself") {

		/*
			procedure main {
				call main;
			}
		*/

		shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
		mainProcedureNode->addChild(mainStmtListNode);

		shared_ptr<ASTNode> callMainNode = CallASTNode::createCallNode();
		callMainNode->addChild(mainProcedureNode);
		mainStmtListNode->addChild(callMainNode);

		testExtractWithError(mainProcedureNode, "A procedure is calling itself!");
	}

	SECTION("Calling non-existent procedure") {
		/*
			procedure main {
				call alpha;
			}
		*/

		shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
		mainProcedureNode->addChild(mainStmtListNode);

		shared_ptr<ASTNode> alphaProcedureNode = ProcedureASTNode::createProcedureNode(alphaToken);

		shared_ptr<ASTNode> callMainNode = CallASTNode::createCallNode();
		callMainNode->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callMainNode);

		testExtractWithError(mainProcedureNode, "Trying to call a non-existent procedure alpha");
	}

}
