#include "catch.hpp"
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/ParentExtractor.h>
#include <sp/design_extractor/ParentTExtractor.h>
#include <sp/design_extractor/UsesExtractor.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/design_extractor/FollowsAndFollowsTExtractor.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>

#include <memory>
#include <unordered_map>
#include <vector>

TEST_CASE("DesignExtractor: test : extractEntity()") {
	auto test = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Entity> expectedEntity) {
		//Given
		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
		shared_ptr<NextExtractor> nextExtractor(new NextExtractor());
		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());
		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor };

		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, *nextExtractor, relationExtractors);

		extractor.setRootNode(nodeToExtractFrom);

		// When
		vector<Entity> entities = extractor.extractEntities(nodeToExtractFrom);

		// Then
		REQUIRE(entities.size() == expectedEntity.size());

		for (int i = 0; i < entities.size(); i++) {
			REQUIRE(entities[i].equals(expectedEntity[i]));
		}
	};

	/*
		procedure main {
		1.	read y;
		2.	x = 1;
		}
	*/
	// Creating tokens
	Token programToken = Token::createProgramToken();
	Token procedureToken = Token::createProcedureToken();
	Token procedureNameToken = Token::createNameOrKeywordToken("main");
	Token stmtLstToken = Token::createPlaceholderToken();
	Token xToken = Token::createNameOrKeywordToken("x");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token constant = Token::createIntegerToken("1");
	Token readToken = Token::createReadToken();
	Token assignToken = Token::createEqualsToken();

	// Creating nodes
	shared_ptr<ASTNode> nodeToExtractFrom = ProgramASTNode::createProgramNode();
	shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(procedureNameToken);
	Entity procedureEntity = Entity::createProcedureEntity(procedureNameToken);

	shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();
	shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(xToken);
	xNode->setLineNumber(2);
	Entity xEntity = Entity::createVariableEntity(2, xToken);

	shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constant);
	constantNode->setLineNumber(2);
	Entity constantEntity = Entity::createConstantEntity(2, constant);


	shared_ptr<ASTNode> assignNode= AssignASTNode::createAssignNode();
	assignNode->setLineNumber(2);
	Entity assignEntity = Entity::createAssignEntity(2);

	shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(yToken);
	yNode->setLineNumber(1);
	Entity yEntity = Entity::createVariableEntity(1, yToken);


	shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();	readNode->setLineNumber(1);
	Entity readEntity = Entity::createReadEntity(1);


	assignNode->addChild(xNode);
	assignNode->addChild(constantNode);

	readNode->addChild(yNode);

	stmtLstNode->addChild(readNode);
	stmtLstNode->addChild(assignNode);

	procedureNode->addChild(stmtLstNode);

	nodeToExtractFrom->addChild(procedureNode);

	// Creating relationships
	vector<Entity> expectedEntities = vector<Entity>{ procedureEntity, readEntity, yEntity, assignEntity, xEntity, constantEntity };

	test(nodeToExtractFrom, expectedEntities);
}

TEST_CASE("DesignExtractor: test : extractRelationships()") {
	auto test = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Relationship> expectedRelationship) {
		// Given
		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
		shared_ptr<NextExtractor> nextExtractor(new NextExtractor());
		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());
		shared_ptr<Extractor<Relationship>> parentExtractor = shared_ptr<Extractor<Relationship>>(new ParentExtractor());
		shared_ptr<Extractor<Relationship>> parentTExtractor = shared_ptr<Extractor<Relationship>>(new ParentTExtractor());
		shared_ptr<Extractor<Relationship>> usesExtractor = shared_ptr<Extractor<Relationship>>(new UsesExtractor());
		shared_ptr<Extractor<Relationship>> followsAndFollowsTExtractor = shared_ptr<Extractor<Relationship>>(new FollowsAndFollowsTExtractor());
		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor, 
			parentExtractor, parentTExtractor, usesExtractor, followsAndFollowsTExtractor };

		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, *nextExtractor, relationExtractors);
		extractor.setRootNode(nodeToExtractFrom);

		// When
		vector<Relationship> relationships = extractor.extractRelationships(nodeToExtractFrom);

		// Then
		REQUIRE(relationships.size() == expectedRelationship.size());
		
		for (int i = 0; i < relationships.size(); i++) {
			
			bool isInExpectedRelationships = false;
			for (int j = 0; j < expectedRelationship.size(); j++) {
				isInExpectedRelationships |= relationships[i].equals(expectedRelationship[j]);
			}
			REQUIRE(isInExpectedRelationships);
		}
	};
	SECTION("Basic procedure") {
		/*
			procedure main {
			1.	read y;
			2.	x = 1;
			}
		*/

		// Creating tokens
		Token programToken = Token::createProgramToken();;
		Token procedureToken = Token::createProcedureToken();;
		Token procedureNameToken = Token::createNameOrKeywordToken("main");
		Token stmtLstToken = Token::createPlaceholderToken();
		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token constant = Token::createIntegerToken("1");
		Token readToken = Token::createReadToken();
		Token assignToken = Token::createEqualsToken();

		// Creating nodes
		shared_ptr<ASTNode> nodeToExtractFrom = ProgramASTNode::createProgramNode();
		shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(procedureNameToken);
		Entity procedureEntity = Entity::createProcedureEntity(procedureNameToken);

		shared_ptr<ASTNode> stmtLstNode = StatementListASTNode::createStatementListNode();
		shared_ptr<ASTNode> xNode = VariableASTNode::createVariableNode(xToken);
		xNode->setLineNumber(2);
		Entity xEntity = Entity::createVariableEntity(2, xToken);

		shared_ptr<ASTNode> constantNode = ConstantValueASTNode::createConstantNode(constant);
		constantNode->setLineNumber(2);
		Entity constantEntity = Entity::createConstantEntity(2, constant);


		shared_ptr<ASTNode> assignNode= AssignASTNode::createAssignNode();
		assignNode->setLineNumber(2);
		Entity assignEntity = Entity::createAssignEntity(2);

		shared_ptr<ASTNode> yNode = VariableASTNode::createVariableNode(yToken);
		yNode->setLineNumber(1);
		Entity yEntity = Entity::createVariableEntity(1, yToken);


		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();		readNode->setLineNumber(1);
		Entity readEntity = Entity::createReadEntity(1);


		assignNode->addChild(xNode);
		assignNode->addChild(constantNode);

		readNode->addChild(yNode);

		stmtLstNode->addChild(readNode);
		stmtLstNode->addChild(assignNode);

		procedureNode->addChild(stmtLstNode);

		nodeToExtractFrom->addChild(procedureNode);

		// Creating relationships
		Relationship procedureModifiesy = Relationship::createModifiesRelationship(procedureEntity, yEntity);
		Relationship procedureModifiesx = Relationship::createModifiesRelationship(procedureEntity, xEntity);
		Relationship readModifiesY = Relationship::createModifiesRelationship(readEntity, yEntity);
		Relationship assignModifiesX = Relationship::createModifiesRelationship(assignEntity, xEntity);

		Relationship readFollowsAssign = Relationship::createFollowsRelationship(readEntity, assignEntity);
		Relationship readFollowsTAssign = Relationship::createFollowsTRelationship(readEntity, assignEntity);


		vector<Relationship> expectedRelation = vector<Relationship>{ procedureModifiesy, procedureModifiesx, readModifiesY, assignModifiesX, readFollowsAssign, readFollowsTAssign };

		test(nodeToExtractFrom, expectedRelation);
	}

	SECTION("Longer procedure with if-else and while statements") {
		/*
		procedure main {
			1. x = 3;
			2. read y;
			3. while ( x != 0 ) {
			4.     x = x - 1
			5.     print y;
			}
			6. if (y > 5) then {
			7.     x = y;
			} else {
			8.     y = x;
			}
		}
	*/
	// Creating tokens
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token assignToken = Token::createEqualsToken();
		Token notEqualToken = Token::createNotEqualsToken();
		Token greaterToken = Token::createGreaterThanToken();
		Token minusToken = Token::createMinusToken();
		Token constThreeToken = Token::createIntegerToken("3");
		Token constZeroToken = Token::createIntegerToken("0");
		Token constOneToken = Token::createIntegerToken("1");
		Token constFiveToken = Token::createIntegerToken("5");
		Token readToken = Token::createReadToken();
		Token whileToken = Token::createWhileToken();
		Token printToken = Token::createPrintToken();
		Token ifToken = Token::createIfToken();
		Token stmtListToken = Token::createPlaceholderToken();

		// Creating AST nodes
		shared_ptr<ASTNode> procedureNode = ProcedureASTNode::createProcedureNode(mainToken);

		// Line 1 (x = 3)
		shared_ptr<ASTNode> mainStmtList = StatementListASTNode::createStatementListNode();		
		shared_ptr<ASTNode> x1Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> assign1Node= AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> constThreeNode = ConstantValueASTNode::createConstantNode(constThreeToken);

		x1Node->setLineNumber(1);
		assign1Node->setLineNumber(1);
		constThreeNode->setLineNumber(1);

		// Line 2 (read y)
		shared_ptr<ASTNode> readNode = ReadASTNode::createReadNode();		
		shared_ptr<ASTNode> y2Node = VariableASTNode::createVariableNode(yToken);

		readNode->setLineNumber(2);
		y2Node->setLineNumber(2);

		// Line 3 (while ( x != 0 ))
		shared_ptr<ASTNode> whileNode = WhileASTNode::createWhileNode();		
		shared_ptr<ASTNode> x3Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> constZeroNode = ConstantValueASTNode::createConstantNode(constZeroToken);
		shared_ptr<ASTNode> notEqualNode = ExpressionASTNode::createExpressionNode(notEqualToken);

		whileNode->setLineNumber(3);
		x3Node->setLineNumber(3);
		constZeroNode->setLineNumber(3);
		notEqualNode->setLineNumber(3);

		// Line 4 (x = x - 1)
		shared_ptr<ASTNode> whileStmtList = StatementListASTNode::createStatementListNode();		
		shared_ptr<ASTNode> x4LhsNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> x4RhsNode = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> assign4Node= AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> minusNode = ExpressionASTNode::createExpressionNode(minusToken);
		shared_ptr<ASTNode> constOneNode = ConstantValueASTNode::createConstantNode(constOneToken);

		x4LhsNode->setLineNumber(4);
		x4RhsNode->setLineNumber(4);
		assign4Node->setLineNumber(4);
		minusNode->setLineNumber(4);
		constOneNode->setLineNumber(4);

		// Line 5 (print y)
		shared_ptr<ASTNode> printNode = PrintASTNode::createPrintNode();		
		shared_ptr<ASTNode> y5Node = VariableASTNode::createVariableNode(yToken);

		printNode->setLineNumber(5);
		y5Node->setLineNumber(5);

		// Line 6 (if (y > 5) then)
		shared_ptr<ASTNode> ifNode = IfASTNode::createIfNode();
		shared_ptr<ASTNode> y6Node = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> greaterNode = ExpressionASTNode::createExpressionNode(greaterToken);
		shared_ptr<ASTNode> constFiveNode = ConstantValueASTNode::createConstantNode(constFiveToken);

		ifNode->setLineNumber(6);
		y6Node->setLineNumber(6);
		greaterNode->setLineNumber(6);
		constFiveNode->setLineNumber(6);

		// Line 7 (x = y)
		shared_ptr<ASTNode> thenStmtList = StatementListASTNode::createStatementListNode();		
		shared_ptr<ASTNode> assign7Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> x7Node = VariableASTNode::createVariableNode(xToken);
		shared_ptr<ASTNode> y7Node = VariableASTNode::createVariableNode(yToken);

		assign7Node->setLineNumber(7);
		x7Node->setLineNumber(7);
		y7Node->setLineNumber(7);

		// Line 8 (y = x)
		shared_ptr<ASTNode> elseStmtList = StatementListASTNode::createStatementListNode();		
		shared_ptr<ASTNode> assign8Node = AssignASTNode::createAssignNode();
		shared_ptr<ASTNode> y8Node = VariableASTNode::createVariableNode(yToken);
		shared_ptr<ASTNode> x8Node = VariableASTNode::createVariableNode(xToken);

		assign8Node->setLineNumber(8);
		y8Node->setLineNumber(8);
		x8Node->setLineNumber(8);

		procedureNode->addChild(mainStmtList);

		mainStmtList->addChild(assign1Node);
		mainStmtList->addChild(readNode);
		mainStmtList->addChild(whileNode);
		mainStmtList->addChild(ifNode);

		assign1Node->addChild(x1Node);
		assign1Node->addChild(constThreeNode);

		readNode->addChild(y2Node);

		whileNode->addChild(notEqualNode);
		whileNode->addChild(whileStmtList);

		notEqualNode->addChild(x3Node);
		notEqualNode->addChild(constZeroNode);

		whileStmtList->addChild(assign4Node);
		whileStmtList->addChild(printNode);

		assign4Node->addChild(x4LhsNode);
		assign4Node->addChild(minusNode);

		minusNode->addChild(x4RhsNode);
		minusNode->addChild(constOneNode);

		printNode->addChild(y5Node);

		ifNode->addChild(greaterNode);
		ifNode->addChild(thenStmtList);
		ifNode->addChild(elseStmtList);

		greaterNode->addChild(y6Node);
		greaterNode->addChild(constFiveNode);

		thenStmtList->addChild(assign7Node);

		assign7Node->addChild(x7Node);
		assign7Node->addChild(y7Node);

		elseStmtList->addChild(assign8Node);

		assign8Node->addChild(y8Node);
		assign8Node->addChild(x8Node);

		// Creating Entities
		Entity procedureEntity = Entity::createProcedureEntity(mainToken);

		Entity assign1Entity = Entity::createAssignEntity(assign1Node->getLineNumber());
		Entity x1Entity = Entity::createVariableEntity(x1Node->getLineNumber(), xToken);
		Entity y2Entity = Entity::createVariableEntity(y2Node->getLineNumber(), yToken);
		Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

		Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());

		Entity assign4Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
		Entity x3Entity = Entity::createVariableEntity(x3Node->getLineNumber(), xToken);
		Entity x4Entity = Entity::createVariableEntity(x4RhsNode->getLineNumber(), xToken);
		Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());
		Entity y5Entity = Entity::createVariableEntity(y5Node->getLineNumber(), yToken);
		Entity y6Entity = Entity::createVariableEntity(y6Node->getLineNumber(), yToken);


		Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());

		Entity assign7Entity = Entity::createAssignEntity(assign7Node->getLineNumber());
		Entity x7Entity = Entity::createVariableEntity(x7Node->getLineNumber(), xToken);
		Entity y7Entity = Entity::createVariableEntity(y7Node->getLineNumber(), yToken);

		Entity assign8Entity = Entity::createAssignEntity(assign8Node->getLineNumber());
		Entity y8Entity = Entity::createVariableEntity(y8Node->getLineNumber(), yToken);
		Entity x8Entity = Entity::createVariableEntity(x8Node->getLineNumber(), xToken);


		// Creating modifies relationships
		Relationship mainModifiesX1 = Relationship::createModifiesRelationship(procedureEntity, x1Entity);
		Relationship mainModifiesY2 = Relationship::createModifiesRelationship(procedureEntity, y2Entity);
		Relationship mainModifiesX4 = Relationship::createModifiesRelationship(procedureEntity, x4Entity);
		Relationship mainModifiesX7 = Relationship::createModifiesRelationship(procedureEntity, x7Entity);
		Relationship mainModifiesY8 = Relationship::createModifiesRelationship(procedureEntity, y8Entity);

		Relationship readModifiesX4 = Relationship::createModifiesRelationship(readEntity, y2Entity);

		Relationship whileModifiesX4 = Relationship::createModifiesRelationship(whileEntity, x4Entity);

		Relationship assign1ModifiesX1 = Relationship::createModifiesRelationship(assign1Entity, x1Entity);

		Relationship assign4ModifiesX4 = Relationship::createModifiesRelationship(assign4Entity, x4Entity);

		Relationship ifModifiesX7 = Relationship::createModifiesRelationship(ifEntity, x7Entity);
		Relationship ifModifiesY8 = Relationship::createModifiesRelationship(ifEntity, y8Entity);

		Relationship assign7ModifiesX7 = Relationship::createModifiesRelationship(assign7Entity, x7Entity);
		Relationship assign8ModifiesY8 = Relationship::createModifiesRelationship(assign8Entity, y8Entity);

		// Create parent relationships
		Relationship whileParentAssign4 = Relationship::createParentRelationship(whileEntity, assign4Entity);
		Relationship whileParentPrint = Relationship::createParentRelationship(whileEntity, printEntity);

		Relationship ifParentAssign7 = Relationship::createParentRelationship(ifEntity, assign7Entity);
		Relationship elseParentAssign8 = Relationship::createParentRelationship(ifEntity, assign8Entity);

		// Create parentT relationships

		Relationship whileParentTAssign4 = Relationship::createParentTRelationship(whileEntity, assign4Entity);
		Relationship whileParentTPrint = Relationship::createParentTRelationship(whileEntity, printEntity);

		Relationship ifParentTAssign7 = Relationship::createParentTRelationship(ifEntity, assign7Entity);
		Relationship elseParentTAssign8 = Relationship::createParentTRelationship(ifEntity, assign8Entity);

		// Create uses relationships

		// Relationship assign1UsesX = Relationship::createUsesRelationship(assign1Entity, x1Entity);

		Relationship whileUsesX3 = Relationship::createUsesRelationship(whileEntity, x3Entity);
		Relationship whileUsesX4 = Relationship::createUsesRelationship(whileEntity, x4Entity);
		Relationship whileUsesY5 = Relationship::createUsesRelationship(whileEntity, y5Entity);
		Relationship assign4UsesX = Relationship::createUsesRelationship(assign4Entity, x4Entity);
		Relationship printUsesY = Relationship::createUsesRelationship(printEntity, y5Entity);

		Relationship ifUsesY6 = Relationship::createUsesRelationship(ifEntity, y6Entity);
		Relationship ifUsesY7 = Relationship::createUsesRelationship(ifEntity, y7Entity);
		Relationship ifUsesX8 = Relationship::createUsesRelationship(ifEntity, x8Entity);

		Relationship assign7UsesY7 = Relationship::createUsesRelationship(assign7Entity, y7Entity);
		Relationship assign8UsesX8 = Relationship::createUsesRelationship(assign8Entity, x8Entity);

		Relationship procUsesX3 = Relationship::createUsesRelationship(procedureEntity, x3Entity);
		Relationship procUsesX4 = Relationship::createUsesRelationship(procedureEntity, x4Entity);
		Relationship procUsesY5 = Relationship::createUsesRelationship(procedureEntity, y5Entity);

		Relationship procUsesY6 = Relationship::createUsesRelationship(procedureEntity, y6Entity);
		Relationship procUsesY7 = Relationship::createUsesRelationship(procedureEntity, y7Entity);
		Relationship procUsesX8 = Relationship::createUsesRelationship(procedureEntity, x8Entity);


		// create Follows relationship
		Relationship assign1FollowsRead = Relationship::createFollowsRelationship(assign1Entity, readEntity);
		Relationship readFollowsWhile = Relationship::createFollowsRelationship(readEntity, whileEntity);
		Relationship whileFollowsIf = Relationship::createFollowsRelationship(whileEntity, ifEntity);
		Relationship assign4FollowsPrint = Relationship::createFollowsRelationship(assign4Entity, printEntity);

		// create FollowsT relationship
		Relationship assign1FollowsTRead = Relationship::createFollowsTRelationship(assign1Entity, readEntity);
		Relationship readFollowsTWhile = Relationship::createFollowsTRelationship(readEntity, whileEntity);
		Relationship whileFollowsTIf = Relationship::createFollowsTRelationship(whileEntity, ifEntity);
		Relationship assign4FollowsTPrint = Relationship::createFollowsTRelationship(assign4Entity, printEntity);

		Relationship assign1FollowsTWhile = Relationship::createFollowsTRelationship(assign1Entity, whileEntity);
		Relationship assign1FollowsTIf = Relationship::createFollowsTRelationship(assign1Entity, ifEntity);

		Relationship readFollowsTIf = Relationship::createFollowsTRelationship(readEntity, ifEntity);


		vector<Relationship> expectedResult = vector<Relationship>{ mainModifiesX1, mainModifiesY2, mainModifiesX4, mainModifiesX7,
																	mainModifiesY8, assign1ModifiesX1, readModifiesX4, whileModifiesX4, assign4ModifiesX4,
																	ifModifiesX7, ifModifiesY8, assign7ModifiesX7, assign8ModifiesY8, whileParentAssign4,
																	whileParentPrint, ifParentAssign7, elseParentAssign8, whileParentTAssign4,
																	whileParentTPrint, ifParentTAssign7, elseParentTAssign8, whileUsesX3,
																	whileUsesX4, whileUsesY5, assign4UsesX, printUsesY, ifUsesY6, ifUsesY7, ifUsesX8, assign7UsesY7, 
																	assign8UsesX8, procUsesX3, procUsesX4, procUsesY5, procUsesY6, procUsesY7, procUsesX8,
																	assign1FollowsRead, readFollowsWhile, whileFollowsIf, assign4FollowsPrint, assign1FollowsTRead,
																	readFollowsTWhile, whileFollowsTIf, assign4FollowsTPrint, assign1FollowsTWhile, assign1FollowsTIf,
																	readFollowsTIf };
		test(procedureNode, expectedResult); 
	}
}

TEST_CASE("Test extractCFGRelationships") {
	auto test = [](vector<shared_ptr<CFGNode>> nodesToExtractFrom, vector<Relationship> expectedRelations) {
		// Given
		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
		shared_ptr<NextExtractor> nextExtractor(new NextExtractor());
		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());
		shared_ptr<Extractor<Relationship>> parentExtractor = shared_ptr<Extractor<Relationship>>(new ParentExtractor());
		shared_ptr<Extractor<Relationship>> parentTExtractor = shared_ptr<Extractor<Relationship>>(new ParentTExtractor());
		shared_ptr<Extractor<Relationship>> usesExtractor = shared_ptr<Extractor<Relationship>>(new UsesExtractor());
		shared_ptr<Extractor<Relationship>> followsAndFollowsTExtractor = shared_ptr<Extractor<Relationship>>(new FollowsAndFollowsTExtractor());
		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor, 
			parentExtractor, parentTExtractor, usesExtractor, followsAndFollowsTExtractor };

		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, *nextExtractor, relationExtractors);

		// When
		vector<Relationship> relationships = extractor.extractCFGRelationships(nodesToExtractFrom);

		// Then
		REQUIRE(relationships.size() == expectedRelations.size());

		for (int i = 0; i < relationships.size(); i++) {
			if (find(relationships.begin(), relationships.end(), expectedRelations[i]) != relationships.end()) {
				REQUIRE(true);
			}
			else {
				Relationship notFound = expectedRelations[i];
				REQUIRE(false);
			}
		}
	};

	// Create CFG Nodes

	// Procedure 1
	/*
		procedure main1 {
		1.	x = 5;
		2.	y = 10;
		3.	print x;
		}
	*/
	unordered_map<int, shared_ptr<CFGNode>> mainOneNodes = {
		{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
		{2, CFGNode::createCFGNode(Entity::createAssignEntity(2))},
		{3, CFGNode::createCFGNode(Entity::createPrintEntity(3))},
	};

	unordered_map<int, vector<int>> mainOneAdjList = {
		{1, {2}},
		{2, {3}},
	};

	shared_ptr<CFGNode> main1 = CFGNode::createCFGFromAdjacencyList(mainOneNodes, mainOneAdjList, 1);

	// Procedure 2
	/*
		procedure main2 {
		4.	if (x != 10) then {
		5.		x = 5;	
			} else {
		6.		y = 10;
			}
		7.	read z;
		}
	*/
	unordered_map<int, shared_ptr<CFGNode>> mainTwoNodes = {
	{4, IfCFGNode::createIfCFGNode(4)},
	// Then
	{5, CFGNode::createCFGNode(Entity::createAssignEntity(5))},
	// Else
	{6, CFGNode::createCFGNode(Entity::createAssignEntity(6))},
	// Outside If
	{7, CFGNode::createCFGNode(Entity::createReadEntity(7))},
	};

	unordered_map<int, vector<int>> mainTwoAdjList = {
		{4, {5,6}},
		{5, {7}},
		{6, {7}},
	};

	shared_ptr<CFGNode> main2 = CFGNode::createCFGFromAdjacencyList(mainTwoNodes, mainTwoAdjList, 4);


	// Procedure 3
	/*
		procedure main3 {
		8.	while (x > z) {
		9.		print z;
		10.		call main2;
			}
		11.	q = z
		}
	*/
	unordered_map<int, shared_ptr<CFGNode>> mainThreeNodes = {
		{8, WhileCFGNode::createWhileCFGNode(8)},
		{9, CFGNode::createCFGNode(Entity::createPrintEntity(9))},
		{10, CFGNode::createCFGNode(Entity::createCallEntity(10))},
		// Outside While
		{11, CFGNode::createCFGNode(Entity::createAssignEntity(11))},
	};

	unordered_map<int, vector<int>> mainThreeAdjList = {
		{8, {9,11}},
		{9, {10}},
		{10, {8}},
	};

	shared_ptr<CFGNode> main3 = CFGNode::createCFGFromAdjacencyList(mainThreeNodes, mainThreeAdjList, 8);

	vector<shared_ptr<CFGNode>> nodesToExtractFrom = {
		main1,
		main2,
		main3,
	};

	vector<Relationship> expectedRelations = {
		Relationship::createNextRelationship(Entity::createAssignEntity(1), Entity::createAssignEntity(2)),
		Relationship::createNextRelationship(Entity::createAssignEntity(2), Entity::createPrintEntity(3)),

		Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(5)),
		Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(6)),
		Relationship::createNextRelationship(Entity::createAssignEntity(5), Entity::createReadEntity(7)),
		Relationship::createNextRelationship(Entity::createAssignEntity(6), Entity::createReadEntity(7)),

		Relationship::createNextRelationship(Entity::createWhileEntity(8), Entity::createPrintEntity(9)),
		Relationship::createNextRelationship(Entity::createWhileEntity(8), Entity::createAssignEntity(11)),
		Relationship::createNextRelationship(Entity::createPrintEntity(9), Entity::createCallEntity(10)),
		Relationship::createNextRelationship(Entity::createCallEntity(10), Entity::createWhileEntity(8)),
	};

	test(nodesToExtractFrom, expectedRelations);
}