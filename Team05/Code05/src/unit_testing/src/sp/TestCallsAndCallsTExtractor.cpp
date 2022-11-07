#include "catch.hpp"

#include <sp/design_extractor/CallsAndCallsTExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>	
#include <memory>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>

using namespace std;

TEST_CASE("CallsAndCallsTExtractor: test extract") {
	auto test = [](shared_ptr<ASTNode> astNode, vector<Relationship> expectedResult) {
		// Given
		CallsAndCallsTExtractor extractor = CallsAndCallsTExtractor();

		// When
		vector<Relationship> extractedRelationships = extractor.extract(astNode);

		// Then
		REQUIRE(extractedRelationships.size() == expectedResult.size());

		sort(extractedRelationships.begin(), extractedRelationships.end(), Relationship::compareRelationship);
		sort(expectedResult.begin(), expectedResult.end(), Relationship::compareRelationship);


		for (int i = 0; i < extractedRelationships.size(); i++)
		{
			bool check = extractedRelationships[i].equals(expectedResult[i]);
			REQUIRE(check);
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
		callAlphaNode->setLineNumber(1);
		callAlphaNode->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callAlphaNode);

		shared_ptr<ASTNode> callMainBetaNode = CallASTNode::createCallNode();
		callMainBetaNode->setLineNumber(2);
		callMainBetaNode->addChild(betaProcedureNode);
		mainStmtListNode->addChild(callMainBetaNode);

		shared_ptr<ASTNode> callAlphaBetaNode = CallASTNode::createCallNode();
		callAlphaBetaNode->setLineNumber(3);
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
		Entity call1 = Entity::createCallEntity(1);
		Entity call2 = Entity::createCallEntity(2);
		Entity call3 = Entity::createCallEntity(3);

		// Create relationships
		Relationship callsMainAlpha = Relationship::createCallsRelationship(mainEntity, alphaEntity);
		Relationship callsMainBeta = Relationship::createCallsRelationship(mainEntity, betaEntity);
		Relationship callsAlphaBeta = Relationship::createCallsRelationship(alphaEntity, betaEntity);
		Relationship callsTMainAlpha = Relationship::createCallsTRelationship(mainEntity, alphaEntity);
		Relationship callsTMainBeta = Relationship::createCallsTRelationship(mainEntity, betaEntity);
		Relationship callsTAlphaBeta = Relationship::createCallsTRelationship(alphaEntity, betaEntity);
		Relationship callsAttribute1Alpha = Relationship::createCallStmtAttributeRelationship(call1, alphaEntity);
		Relationship callsAttribute2Beta = Relationship::createCallStmtAttributeRelationship(call2, betaEntity);
		Relationship callsAttribute3Beta = Relationship::createCallStmtAttributeRelationship(call3, betaEntity);

		vector<Relationship> expectedCallsAndCallsTRelationships{ 
				callsMainAlpha, callsTMainAlpha, callsAttribute1Alpha,
				callsMainBeta, callsTMainBeta, callsAttribute2Beta, 
				callsAlphaBeta, callsTAlphaBeta, callsAttribute3Beta, };

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
		callAlpha1Node->setLineNumber(1);
		callAlpha1Node->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callAlpha1Node);

		mainStmtListNode->addChild(readNode);

		shared_ptr<ASTNode> callAlpha2Node = CallASTNode::createCallNode();
		callAlpha2Node->setLineNumber(3);
		callAlpha2Node->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callAlpha2Node);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);

		// Create entities
		Entity mainEntity = Entity::createProcedureEntity(mainToken);
		Entity alphaEntity = Entity::createProcedureEntity(alphaToken);
		Entity call1 = Entity::createCallEntity(1);
		Entity call3 = Entity::createCallEntity(3);

		// Create relationships
		Relationship callsMainAlpha = Relationship::createCallsRelationship(mainEntity, alphaEntity);
		Relationship callsTMainAlpha = Relationship::createCallsTRelationship(mainEntity, alphaEntity);
		Relationship callsAttribute1Alpha = Relationship::createCallStmtAttributeRelationship(call1, alphaEntity);
		Relationship callsAttribute3Alpha = Relationship::createCallStmtAttributeRelationship(call3, alphaEntity);

		vector<Relationship> expectedCallsAndCallsTRelationships{ callsMainAlpha, callsTMainAlpha, callsAttribute1Alpha,
				callsAttribute3Alpha, };

		test(programNode, expectedCallsAndCallsTRelationships);
	}

	SECTION("Nested calls") {
		/*
			procedure main {
				read y;
				if (y > 0) then {
					call alpha;
				} else {
					call beta;
				}
			}

			procedure alpha {
				read x;
				if (x > 0) then {
					while (x > 0) {
						call beta;
						x = x - 1;
					}
				} else {}
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

		shared_ptr<ASTNode> readYNode = ReadASTNode::createReadNode();
		mainStmtListNode->addChild(readYNode);

		shared_ptr<ASTNode> mainIfNode = IfASTNode::createIfNode();
		shared_ptr<ASTNode> mainIfCond = ExpressionASTNode::createExpressionNode(Token::createGreaterThanToken());
		shared_ptr<ASTNode> thenStmtListNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> elseStmtListNode = StatementListASTNode::createStatementListNode();
		mainIfNode->addChild(mainIfCond);
		mainIfNode->addChild(thenStmtListNode);
		mainIfNode->addChild(elseStmtListNode);
		mainStmtListNode->addChild(mainIfNode);

		shared_ptr<ASTNode> callMainAlphaNode = CallASTNode::createCallNode();
		callMainAlphaNode->setLineNumber(3);
		callMainAlphaNode->addChild(alphaProcedureNode);
		thenStmtListNode->addChild(callMainAlphaNode);

		shared_ptr<ASTNode> callMainBetaNode = CallASTNode::createCallNode();
		callMainBetaNode->setLineNumber(4);
		callMainBetaNode->addChild(betaProcedureNode);
		elseStmtListNode->addChild(callMainBetaNode);

		shared_ptr<ASTNode> readXNode = ReadASTNode::createReadNode();
		alphaStmtListNode->addChild(readXNode);

		shared_ptr<ASTNode> alphaIfNode = IfASTNode::createIfNode();
		shared_ptr<ASTNode> alphaIfCond = ExpressionASTNode::createExpressionNode(Token::createGreaterThanToken());
		shared_ptr<ASTNode> thenStmtListNode2 = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> elseStmtListNode2 = StatementListASTNode::createStatementListNode();
		alphaIfNode->addChild(alphaIfCond);
		alphaIfNode->addChild(thenStmtListNode2);
		alphaIfNode->addChild(elseStmtListNode2);
		alphaStmtListNode->addChild(alphaIfNode);

		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();
		shared_ptr<ASTNode> whileCond = ExpressionASTNode::createExpressionNode(Token::createGreaterThanToken());
		shared_ptr<ASTNode> whileStmtListNode = StatementListASTNode::createStatementListNode();
		whileNode->addChild(whileCond);
		whileNode->addChild(whileStmtListNode);
		thenStmtListNode2->addChild(whileNode);

		shared_ptr<ASTNode> callAlphaBetaNode = CallASTNode::createCallNode();
		callAlphaBetaNode->setLineNumber(8);
		callAlphaBetaNode->addChild(betaProcedureNode);
		whileStmtListNode->addChild(callAlphaBetaNode);

		shared_ptr<ASTNode> assignNode = AssignASTNode::createAssignNode();
		whileStmtListNode->addChild(assignNode);

		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
		programNode->addChild(mainProcedureNode);
		programNode->addChild(alphaProcedureNode);
		programNode->addChild(betaProcedureNode);

		// Create entities
		Entity mainEntity = Entity::createProcedureEntity(mainToken);
		Entity alphaEntity = Entity::createProcedureEntity(alphaToken);
		Entity betaEntity = Entity::createProcedureEntity(betaToken);
		Entity call3 = Entity::createCallEntity(3);
		Entity call4 = Entity::createCallEntity(4);
		Entity call8 = Entity::createCallEntity(8);

		// Create relationships
		Relationship callsMainAlpha = Relationship::createCallsRelationship(mainEntity, alphaEntity);
		Relationship callsTMainAlpha = Relationship::createCallsTRelationship(mainEntity, alphaEntity);
		Relationship callsMainBeta = Relationship::createCallsRelationship(mainEntity, betaEntity);
		Relationship callsTMainBeta = Relationship::createCallsTRelationship(mainEntity, betaEntity);
		Relationship callsAlphaBeta = Relationship::createCallsRelationship(alphaEntity, betaEntity);
		Relationship callsTAlphaBeta = Relationship::createCallsTRelationship(alphaEntity, betaEntity);
		Relationship callsAttribute3Alpha = Relationship::createCallStmtAttributeRelationship(call3, alphaEntity);
		Relationship callsAttribute4Beta = Relationship::createCallStmtAttributeRelationship(call4, betaEntity);
		Relationship callsAttribute8Beta = Relationship::createCallStmtAttributeRelationship(call8, betaEntity);

		vector<Relationship> expectedCallsAndCallsTRelationships{ callsMainAlpha, callsTMainAlpha, callsAttribute3Alpha,
				callsMainBeta, callsTMainBeta, callsAttribute4Beta, callsAlphaBeta, callsTAlphaBeta, callsAttribute8Beta };

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
		callMainAlphaNode->setLineNumber(1);
		callMainAlphaNode->addChild(alphaProcedureNode);
		mainStmtListNode->addChild(callMainAlphaNode);

		shared_ptr<ASTNode> callMainBetaNode = CallASTNode::createCallNode();
		callMainBetaNode->setLineNumber(2);
		callMainBetaNode->addChild(betaProcedureNode);
		mainStmtListNode->addChild(callMainBetaNode);

		shared_ptr<ASTNode> callAlphaCharlieNode = CallASTNode::createCallNode();
		callAlphaCharlieNode->setLineNumber(3);
		callAlphaCharlieNode->addChild(charlieProcedureNode);
		alphaStmtListNode->addChild(callAlphaCharlieNode);

		shared_ptr<ASTNode> callBetaDeltaNode = CallASTNode::createCallNode();
		callBetaDeltaNode->setLineNumber(4);
		callBetaDeltaNode->addChild(deltaProcedureNode);
		betaStmtListNode->addChild(callBetaDeltaNode);

		shared_ptr<ASTNode> callDeltaCharlieNode = CallASTNode::createCallNode();
		callDeltaCharlieNode->setLineNumber(5);
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
		Entity call1 = Entity::createCallEntity(1);
		Entity call2 = Entity::createCallEntity(2);
		Entity call3 = Entity::createCallEntity(3);
		Entity call4= Entity::createCallEntity(4);
		Entity call5 = Entity::createCallEntity(5);

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

		Relationship callsAttribute1Alpha = Relationship::createCallStmtAttributeRelationship(call1, alphaEntity);
		Relationship callsAttribute2Beta = Relationship::createCallStmtAttributeRelationship(call2, betaEntity);
		Relationship callsAttribute3Charlie = Relationship::createCallStmtAttributeRelationship(call3, charlieEntity);
		Relationship callsAttribute4Delta = Relationship::createCallStmtAttributeRelationship(call4, deltaEntity);
		Relationship callsAttribute5Charlie = Relationship::createCallStmtAttributeRelationship(call5, charlieEntity);

		vector<Relationship> expectedCallsAndCallsTRelationships{ 
				callsAttribute1Alpha, callsAttribute2Beta, callsAttribute3Charlie, callsAttribute4Delta, callsAttribute5Charlie,
				callsTMainAlpha, callsTMainCharlie, callsTMainBeta, callsTMainDelta,
				callsTAlphaCharlie, callsTBetaDelta, callsTBetaCharlie,
				callsTDeltaCharlie, 
				callsMainAlpha, callsMainBeta,
				callsAlphaCharlie, callsBetaDelta, callsDeltaCharlie
		};

		test(programNode, expectedCallsAndCallsTRelationships);
	}


	SECTION("Test Milestone 2 buggy program") {
		/*
			procedure main {
				call a;
				call b;
			}

			procedure a {
				call b;
				call e;
			}

			procedure b {
				read x;
			}

			procedure e {
				read x;
			}
		*/
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token aToken = Token::createNameOrKeywordToken("a");
		Token bToken = Token::createNameOrKeywordToken("b");
		Token eToken = Token::createNameOrKeywordToken("e");

        shared_ptr<ASTNode> mainProcedureNode = ProcedureASTNode::createProcedureNode(mainToken);
		shared_ptr<ASTNode> aProcedureNode = ProcedureASTNode::createProcedureNode(aToken);
		shared_ptr<ASTNode> bProcedureNode = ProcedureASTNode::createProcedureNode(bToken);
		shared_ptr<ASTNode> eProcedureNode = ProcedureASTNode::createProcedureNode(eToken);
		
		// Procedure main 
		shared_ptr<ASTNode> mainStmtListNode = StatementListASTNode::createStatementListNode();
        mainProcedureNode->addChild(mainStmtListNode);

		shared_ptr<ASTNode> mainCallA = CallASTNode::createCallNode();
		mainCallA->setLineNumber(1);
        mainCallA->addChild(aProcedureNode);

		shared_ptr<ASTNode> mainCallB = CallASTNode::createCallNode();
		mainCallB->setLineNumber(2);
        mainCallB->addChild(bProcedureNode);

        mainStmtListNode->addChild(mainCallA);
        mainStmtListNode->addChild(mainCallB);


		// Procedure A
		shared_ptr<ASTNode> aStmtListNode = StatementListASTNode::createStatementListNode();
		aProcedureNode->addChild(aStmtListNode);
        
		shared_ptr<ASTNode> aCallB = CallASTNode::createCallNode();
        aCallB->setLineNumber(3);
        aCallB->addChild(bProcedureNode);
        
        shared_ptr<ASTNode> aCallE = CallASTNode::createCallNode();
        aCallE->setLineNumber(4);
		aCallE->addChild(eProcedureNode);
        
		aStmtListNode->addChild(aCallB);
		aStmtListNode->addChild(aCallE);


		// Procedure B
		shared_ptr<ASTNode> bStmtListNode = StatementListASTNode::createStatementListNode();
        bProcedureNode->addChild(bStmtListNode);
        
		shared_ptr<ASTNode> readX = ReadASTNode::createReadNode();
		readX->setLineNumber(5);
		bStmtListNode->addChild(readX);
        
		// Procedure E
		shared_ptr<ASTNode> eStmtListNode = StatementListASTNode::createStatementListNode();
        eProcedureNode->addChild(eStmtListNode);
        
		shared_ptr<ASTNode> readX2 = ReadASTNode::createReadNode();
		readX->setLineNumber(6);
		eStmtListNode->addChild(readX2);


		shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
        programNode->addChild(mainProcedureNode);
		programNode->addChild(aProcedureNode);
		programNode->addChild(bProcedureNode);
		programNode->addChild(eProcedureNode);

        // Entities
        Entity mainEntity = Entity::createProcedureEntity(mainToken);
        Entity aEntity = Entity::createProcedureEntity(aToken);
        Entity bEntity = Entity::createProcedureEntity(bToken);
        Entity eEntity = Entity::createProcedureEntity(eToken);
        Entity call1 = Entity::createCallEntity(1);
        Entity call2 = Entity::createCallEntity(2);
        Entity call3 = Entity::createCallEntity(3);
        Entity call4 = Entity::createCallEntity(4);

		vector<Relationship> expectedCallsAndCallsTRelationships{
			Relationship::createCallsRelationship(mainEntity, aEntity),
			Relationship::createCallsRelationship(mainEntity, bEntity),

			Relationship::createCallsRelationship(aEntity, bEntity),
			Relationship::createCallsRelationship(aEntity, eEntity),

            Relationship::createCallStmtAttributeRelationship(call1, aEntity),
            Relationship::createCallStmtAttributeRelationship(call2, bEntity),
            Relationship::createCallStmtAttributeRelationship(call3, bEntity),
            Relationship::createCallStmtAttributeRelationship(call4, eEntity),
            
            Relationship::createCallsTRelationship(mainEntity, aEntity),
            Relationship::createCallsTRelationship(mainEntity, bEntity),
            Relationship::createCallsTRelationship(mainEntity, eEntity),
            
            Relationship::createCallsTRelationship(aEntity, bEntity),
            Relationship::createCallsTRelationship(aEntity, eEntity),
		};

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

        shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
        programNode->addChild(mainProcedureNode);

		testExtractWithError(programNode, "A procedure is calling itself!");
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

        shared_ptr<ASTNode> programNode = ProgramASTNode::createProgramNode();
        programNode->addChild(mainProcedureNode);

		testExtractWithError(programNode, "Trying to call a non-existent procedure alpha");
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

		testExtractWithError(programNode, "Procedure main is calling itself");
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

		testExtractWithError(programNode, "Procedure main is calling itself");
	}

}
