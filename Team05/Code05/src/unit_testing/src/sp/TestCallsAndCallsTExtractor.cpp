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
#include <sp/dataclasses/ast/ReadASTNode.h>
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
			if (extractedRelationships[i].isCalls()) {

			}

			REQUIRE(extractedRelationships[i].equals(expectedResult[i]));
		}
	};

	// Variables are omitted for brevity. CallsAndCallsTExtractor does not interact with variables.
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token alphaToken = Token::createNameOrKeywordToken("alpha");
	Token betaToken = Token::createNameOrKeywordToken("beta");
	Token charlieToken = Token::createNameOrKeywordToken("charlie");
	Token deltaToken = Token::createNameOrKeywordToken("delta");
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
		Relationship callsTMainAlpha = Relationship::createCallsTRelationship(mainEntity, alphaEntity);
		Relationship callsTMainBeta = Relationship::createCallsTRelationship(mainEntity, betaEntity);
		Relationship callsTAlphaBeta = Relationship::createCallsTRelationship(alphaEntity, betaEntity);

		vector<Relationship> expectedCallsAndCallsTRelationships{ callsMainAlpha, callsTMainAlpha, callsMainBeta,
				callsTMainBeta, callsAlphaBeta, callsTAlphaBeta };

		test(programNode, expectedCallsAndCallsTRelationships);
	}	

	SECTION("Simple program with duplicate calls") {

		/*
			procedure main {
				call alpha;
				read y;
				call alpha;
			}

			procedure alpha {}
		*/

		shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
		mainProcedureNode->addChild(mainStmtListNode);

		shared_ptr<ASTNode> alphaProcedureNode = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> alphaStmtListNode = StatementListASTNode::createStatementListNode();
		alphaProcedureNode->addChild(alphaStmtListNode);

		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();
		
		shared_ptr<ASTNode> callAlpha1Node = CallASTNode::createCallNode();
		callAlpha1Node->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callAlpha1Node);

		mainStmtListNode->addChild(readNode);

		shared_ptr<ASTNode> callAlpha2Node = CallASTNode::createCallNode();
		callAlpha2Node->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callAlpha2Node);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);

		// Create entities
		Entity mainEntity = Entity::createProcedureEntity(mainToken);
		Entity alphaEntity = Entity::createProcedureEntity(alphaToken);

		// Create relationships
		Relationship callsMainAlpha = Relationship::createCallsRelationship(mainEntity, alphaEntity);
		Relationship callsTMainAlpha = Relationship::createCallsTRelationship(mainEntity, alphaEntity);

		vector<Relationship> expectedCallsAndCallsTRelationships{ callsMainAlpha, callsTMainAlpha };

		test(programNode, expectedCallsAndCallsTRelationships);
	}

	SECTION("Complex program") {
		/*
			procedure main {
				call alpha;
				call beta;
			}

			procedure alpha {
				call charlie;
			}

			procedure beta {
				call delta;
			}

			procedure charlie {}

			procedure delta {
				call charlie;
			}
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

		// procedure charlie
		shared_ptr<ASTNode> charlieProcedureNode = ProcedureASTNode::createProcedureNode(charlieToken);
		shared_ptr<ASTNode> charlieStmtListNode = StatementListASTNode::createStatementListNode();
		charlieProcedureNode->addChild(charlieStmtListNode);

		// procedure delta
		shared_ptr<ASTNode> deltaProcedureNode = ProcedureASTNode::createProcedureNode(deltaToken);
		shared_ptr<ASTNode> deltaStmtListNode = StatementListASTNode::createStatementListNode();
		deltaProcedureNode->addChild(deltaStmtListNode);

		shared_ptr<ASTNode> callMainAlphaNode = CallASTNode::createCallNode();
		callMainAlphaNode->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callMainAlphaNode);

		shared_ptr<ASTNode> callMainBetaNode = CallASTNode::createCallNode();
		callMainBetaNode->addChild(betaProcedureNode);
		mainStmtListNode->addChild(callMainBetaNode);

		shared_ptr<ASTNode> callAlphaCharlieNode = CallASTNode::createCallNode();
		callAlphaCharlieNode->addChild(charlieProcedureNode);
		alphaStmtListNode->addChild(callAlphaCharlieNode);

		shared_ptr<ASTNode> callBetaDeltaNode = CallASTNode::createCallNode();
		callBetaDeltaNode->addChild(deltaProcedureNode);
		betaStmtListNode->addChild(callBetaDeltaNode);

		shared_ptr<ASTNode> callDeltaCharlieNode = CallASTNode::createCallNode();
		callDeltaCharlieNode->addChild(charlieProcedureNode);
		deltaStmtListNode->addChild(callDeltaCharlieNode);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);
		programNode->addChild(betaProcedureNode);
		programNode->addChild(charlieProcedureNode);
		programNode->addChild(deltaProcedureNode);

		// Create entities
		Entity mainEntity = Entity::createProcedureEntity(mainToken);
		Entity alphaEntity = Entity::createProcedureEntity(alphaToken);
		Entity betaEntity = Entity::createProcedureEntity(betaToken);
		Entity charlieEntity = Entity::createProcedureEntity(charlieToken);
		Entity deltaEntity = Entity::createProcedureEntity(deltaToken);

		// Create relationships
		Relationship callsMainAlpha = Relationship::createCallsRelationship(mainEntity, alphaEntity);
		Relationship callsMainBeta = Relationship::createCallsRelationship(mainEntity, betaEntity);
		Relationship callsAlphaCharlie = Relationship::createCallsRelationship(alphaEntity, charlieEntity);
		Relationship callsBetaDelta = Relationship::createCallsRelationship(betaEntity, deltaEntity);
		Relationship callsDeltaCharlie = Relationship::createCallsRelationship(deltaEntity, charlieEntity);

		Relationship callsTMainAlpha = Relationship::createCallsTRelationship(mainEntity, alphaEntity);
		Relationship callsTMainBeta = Relationship::createCallsTRelationship(mainEntity, betaEntity);
		Relationship callsTAlphaCharlie = Relationship::createCallsTRelationship(alphaEntity, charlieEntity);
		Relationship callsTBetaDelta = Relationship::createCallsTRelationship(betaEntity, deltaEntity);
		Relationship callsTDeltaCharlie = Relationship::createCallsTRelationship(deltaEntity, charlieEntity);

		Relationship callsTMainCharlie = Relationship::createCallsTRelationship(mainEntity, charlieEntity);
		Relationship callsTMainDelta = Relationship::createCallsTRelationship(mainEntity, deltaEntity);
		Relationship callsTBetaCharlie = Relationship::createCallsTRelationship(betaEntity, charlieEntity);

		vector<Relationship> expectedCallsAndCallsTRelationships{ callsMainAlpha, callsTMainAlpha, callsMainBeta, callsTMainBeta,
				callsAlphaCharlie, callsTAlphaCharlie, callsBetaDelta, callsTBetaDelta, callsDeltaCharlie, callsTDeltaCharlie,
				callsTMainCharlie, callsTMainDelta, callsTBetaCharlie };

		test(programNode, expectedCallsAndCallsTRelationships);
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
	Token charlieToken = Token::createNameOrKeywordToken("charlie");
	Token deltaToken = Token::createNameOrKeywordToken("delta");
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

	SECTION("Procedures with same name") {
		/*
			procedure main {
				call alpha;
			}

			procedure alpha {}

			procedure alpha {}
		*/

		shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
		mainProcedureNode->addChild(mainStmtListNode);

		shared_ptr<ASTNode> alphaProcedureNode1 = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> alphaStmtListNode1 = StatementListASTNode::createStatementListNode();
		alphaProcedureNode1->addChild(alphaStmtListNode1);

		shared_ptr<ASTNode> alphaProcedureNode2 = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> alphaStmtListNode2 = StatementListASTNode::createStatementListNode();
		alphaProcedureNode2->addChild(alphaStmtListNode2);

		shared_ptr<ASTNode> callAlphaNode = CallASTNode::createCallNode();
		callAlphaNode->addChild(alphaProcedureNode1);
		mainStmtListNode->addChild(callAlphaNode);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode1);
		programNode->addChild(alphaProcedureNode2);

		testExtractWithError(programNode, "Procedure alpha was declared twice in the program");
	}

	SECTION("Simple cyclic call") {
		/*
			procedure main {
				call alpha;
			}

			procedure alpha {
				call main;
			}
		*/
		shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
		mainProcedureNode->addChild(mainStmtListNode);

		shared_ptr<ASTNode> alphaProcedureNode = ProcedureASTNode::createProcedureNode(alphaToken);
		shared_ptr<ASTNode> alphaStmtListNode = StatementListASTNode::createStatementListNode();
		alphaProcedureNode->addChild(alphaStmtListNode);

		shared_ptr<ASTNode> callAlphaNode = CallASTNode::createCallNode();
		callAlphaNode->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callAlphaNode);

		shared_ptr<ASTNode> callMainNode = CallASTNode::createCallNode();
		callMainNode->addChild(mainProcedureNode);
		alphaStmtListNode->addChild(callMainNode);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);

		testExtractWithError(programNode, "The program contains cyclic procedure calls! This is not allowed");
	}

	SECTION("Complex cyclic call") {
		/*
			procedure main {
				call alpha;
				call beta;
			}

			procedure alpha {
				call charlie;
			}

			procedure beta {
				call delta;
			}

			procedure charlie {
				call main;
			}

			procedure delta {
				call charlie;
			}
			
			The cyclic calls are: 
			main -> alpha -> charlie -> main
			main -> beta -> delta -> charlie -> main
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

		// procedure charlie
		shared_ptr<ASTNode> charlieProcedureNode = ProcedureASTNode::createProcedureNode(charlieToken);
		shared_ptr<ASTNode> charlieStmtListNode = StatementListASTNode::createStatementListNode();
		charlieProcedureNode->addChild(charlieStmtListNode);

		// procedure delta
		shared_ptr<ASTNode> deltaProcedureNode = ProcedureASTNode::createProcedureNode(deltaToken);
		shared_ptr<ASTNode> deltaStmtListNode = StatementListASTNode::createStatementListNode();
		deltaProcedureNode->addChild(deltaStmtListNode);

		shared_ptr<ASTNode> callMainAlphaNode = CallASTNode::createCallNode();
		callMainAlphaNode->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callMainAlphaNode);

		shared_ptr<ASTNode> callMainBetaNode = CallASTNode::createCallNode();
		callMainBetaNode->addChild(betaProcedureNode);
		mainStmtListNode->addChild(callMainBetaNode);

		shared_ptr<ASTNode> callAlphaCharlieNode = CallASTNode::createCallNode();
		callAlphaCharlieNode->addChild(charlieProcedureNode);
		alphaStmtListNode->addChild(callAlphaCharlieNode);

		shared_ptr<ASTNode> callBetaDeltaNode = CallASTNode::createCallNode();
		callBetaDeltaNode->addChild(deltaProcedureNode);
		betaStmtListNode->addChild(callBetaDeltaNode);

		shared_ptr<ASTNode> callCharlieMainNode = CallASTNode::createCallNode();
		callCharlieMainNode->addChild(mainProcedureNode);
		charlieStmtListNode->addChild(callCharlieMainNode);

		shared_ptr<ASTNode> callDeltaCharlieNode = CallASTNode::createCallNode();
		callDeltaCharlieNode->addChild(charlieProcedureNode);
		deltaStmtListNode->addChild(callDeltaCharlieNode);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);
		programNode->addChild(betaProcedureNode);
		programNode->addChild(charlieProcedureNode);
		programNode->addChild(deltaProcedureNode);

		testExtractWithError(programNode, "The program contains cyclic procedure calls! This is not allowed");
	}
}
