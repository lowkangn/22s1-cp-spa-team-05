#include "catch.hpp"
#include <sp/SourceProcessor.h>
#include <istream>
#include <sstream>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <pkb/PKB.h>
#include <pkb/pkbRepository/table_managers/PkbEntityTable.h>
#include <pkb/pkbRepository/table_managers/PkbRelationshipTable.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/design_extractor/ParentExtractor.h>
#include <sp/design_extractor/ParentTExtractor.h>
#include <sp/design_extractor/UsesExtractor.h>
#include <memory>
#include <vector>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/FollowsAndFollowsTExtractor.h>
#include <sp/design_extractor/CallsAndCallsTExtractor.h>
#include <sp/SPException.h>
#include <string>
#include <list>

using namespace std;

TEST_CASE("Test SP extraction of Entities and Relationships") {
	auto testExtract = [](string program, vector<Entity> expectedEntities, vector<Relationship> expectedRelationships, vector<Pattern> expectedPatterns) {
		// Given
		stringstream ss(program);
		istream& stream = ss;

		Lexer lexer = Lexer();
		list<Token> tokens = lexer.tokenize(stream);

		ParserManager parser = ParserManager();
		shared_ptr<ASTNode> programTree = parser.parse(tokens);
		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
		shared_ptr<NextExtractor> nextExtractor(new NextExtractor());
		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());
		shared_ptr<Extractor<Relationship>> parentExtractor = shared_ptr<Extractor<Relationship>>(new ParentExtractor());
		shared_ptr<Extractor<Relationship>> parentTExtractor = shared_ptr<Extractor<Relationship>>(new ParentTExtractor());
		shared_ptr<Extractor<Relationship>> usesExtractor = shared_ptr<Extractor<Relationship>>(new UsesExtractor());
		shared_ptr<Extractor<Relationship>> followsAndFollowsTExtractor = shared_ptr<Extractor<Relationship>>(new FollowsAndFollowsTExtractor());
		shared_ptr<Extractor<Relationship>> callsAndCallsTExtractor = shared_ptr<Extractor<Relationship>>(new CallsAndCallsTExtractor());
		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor, 
				parentExtractor, parentTExtractor, usesExtractor, followsAndFollowsTExtractor, callsAndCallsTExtractor };
		
		// create manager
		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, *nextExtractor, relationExtractors);

		vector<Entity> extractedEntities = extractor.extractEntities(programTree);
		vector<Relationship> extractedRelationships = extractor.extractRelationships(programTree);
		vector<Pattern> extractedPatterns = extractor.extractPatterns(programTree);
		
		REQUIRE(extractedEntities.size() == expectedEntities.size());
		REQUIRE(extractedRelationships.size() == expectedRelationships.size());
		REQUIRE(extractedPatterns.size() == expectedPatterns.size());
		
		
		for (int i = 0; i < extractedEntities.size(); i++) {
			bool isInExpectedEntities = false;
			for (int j = 0; j < expectedEntities.size(); j++) {
				if (extractedEntities[i].equals(expectedEntities[j])) {
					isInExpectedEntities = true;
					break;
				}
			}
			REQUIRE(isInExpectedEntities);
		}
		
		for (int i = 0; i < extractedRelationships.size(); i++) {
			bool isInExpectedRelationships = false;
			for (int j = 0; j < expectedRelationships.size(); j++) {
				if (extractedRelationships[i].equals(expectedRelationships[j])) {
					isInExpectedRelationships = true;
					break;
				}
			}
			
			REQUIRE(isInExpectedRelationships);
		}
		
		
		for (int i = 0; i < extractedPatterns.size(); i++) {
			bool isInExpectedPatterns = false;
			for (int j = 0; j < expectedPatterns.size(); j++) {
				if (extractedPatterns[i].equals(expectedPatterns[j])) {
					isInExpectedPatterns = true;
					break;
				}
			}
			REQUIRE(isInExpectedPatterns);
		}
		
	};
	
	SECTION("Simple procedure with print statement") {
		string program = "procedure main {\nprint x;\n}";
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xTokenMain = Token::createNameOrKeywordToken("x");

		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity printEntity = Entity::createPrintEntity(1);

		vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, xEntity, printEntity };

		// relationships 
		Relationship mainUsesXRelationship = Relationship::createUsesRelationship(procedureMainEntity, xEntity);
		Relationship printUsesXRelationship = Relationship::createUsesRelationship(printEntity, xEntity);

		vector<Relationship> expectedRelationships = vector<Relationship>{ mainUsesXRelationship, printUsesXRelationship };

		vector<Pattern> expectedPatterns{ };

		testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);

	}
	
	
	SECTION("Test procedure with print statement in a while loop") {
		/*
			procedure main {
			1.	while (x > 0) {
			2.		print y;
				}
			}
		*/
	
		string program = "procedure main {\nwhile (x > 0) {\nprint y;\n}}";

		// Tokens
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xTokenMain = Token::createNameOrKeywordToken("x");
		Token yTokenMain = Token::createNameOrKeywordToken("y");
		Token constToken = Token::createIntegerToken("0");
		Token greaterThanToken = Token::createGreaterThanToken();


		// Entities
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity notEqualEntity = Entity::createExpressionEntity(1, greaterThanToken);
		Entity constEntity = Entity::createConstantEntity(1, constToken);
		Entity whileEntity = Entity::createWhileEntity(1);

		Entity printEntity = Entity::createPrintEntity(2);
		Entity yEntity = Entity::createVariableEntity(2, yTokenMain);

		vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, xEntity, notEqualEntity, constEntity, whileEntity,
														printEntity, yEntity };

		// Relationships
		Relationship mainUsesX = Relationship::createUsesRelationship(procedureMainEntity, xEntity);
		Relationship mainUsesY = Relationship::createUsesRelationship(procedureMainEntity, yEntity);
		Relationship whileParentPrint = Relationship::createParentRelationship(whileEntity, printEntity);
		Relationship whileParentTPrint = Relationship::createParentTRelationship(whileEntity, printEntity);
		Relationship whileUsesX = Relationship::createUsesRelationship(whileEntity, xEntity);
		Relationship whileUsesY = Relationship::createUsesRelationship(whileEntity, yEntity);
		Relationship printUsesY = Relationship::createUsesRelationship(printEntity, yEntity);

		vector<Relationship> expectedRelationships = vector<Relationship>{ mainUsesX, mainUsesY, whileParentPrint, whileParentTPrint,
																		 whileUsesX, whileUsesY, printUsesY };

		vector<Pattern> expectedPatterns{ Pattern::createWhilePattern(1, "x") };

		testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);
	}
	

	
	SECTION("Basic program") {
		string program = "procedure main {\nx = 1;\nread y;}";
		// Create tokens
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xTokenMain = Token::createNameOrKeywordToken("x");
		Token assignToken = Token::createEqualsToken();
		Token constToken = Token::createIntegerToken("1");
		Token readToken = Token::createReadToken();
		Token yToken = Token::createNameOrKeywordToken("y");

		// Create entities
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(1);
		Entity constEntity = Entity::createConstantEntity(1, constToken);

		Entity yEntity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, xEntity, assignEntity, constEntity, readEntity, yEntity };

		// Create relationships
		Relationship procedureXRelationship = Relationship::createModifiesRelationship(procedureMainEntity, xEntity);
		Relationship procedureYRelationship = Relationship::createModifiesRelationship(procedureMainEntity, yEntity);
		Relationship assignRelation = Relationship::createModifiesRelationship(assignEntity, xEntity);
		Relationship readRelation = Relationship::createModifiesRelationship(readEntity, yEntity);

		Relationship assignFollowsRead = Relationship::createFollowsRelationship(assignEntity, readEntity);
		Relationship assignFollowsTRead = Relationship::createFollowsTRelationship(assignEntity, readEntity);

		vector<Relationship> expectedRelationships = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation, assignFollowsRead, assignFollowsTRead };
		
		vector<Pattern> expectedPatterns{ Pattern::createAssignPattern(1, " x ", " 1 ") };

		testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);
	}

	

	SECTION("Multi-procedure program") {
		/*
			procedure main {
				1. x = 1;
			}

			procedure readY {
				2. read y;
			}
		*/
	
		string program = "procedure main {\nx=1;\n}\n procedure readY {\nread y;\n}";
		// Create tokens
		// main
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xTokenMain = Token::createNameOrKeywordToken("x");
		Token assignToken = Token::createEqualsToken();
		Token constToken = Token::createIntegerToken("1");

		// readY
		Token readYToken = Token::createNameOrKeywordToken("readY");
		Token readToken = Token::createReadToken();
		Token yToken = Token::createNameOrKeywordToken("y");

		// Create entities
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
		Entity procedureReadYEntity = Entity::createProcedureEntity(readYToken);

		Entity statementListMain = Entity::createStmtLstEntity();
		Entity statementListReadY = Entity::createStmtLstEntity();

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(1);
		Entity constEntity = Entity::createConstantEntity(1, constToken);

		Entity yEntity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		Entity fakeEntity = Entity::createReadEntity(500);


		vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, procedureReadYEntity, xEntity, assignEntity, constEntity, readEntity, yEntity };

		// Create relationships
		Relationship procedureXRelationship = Relationship::createModifiesRelationship(procedureMainEntity, xEntity);
		Relationship procedureYRelationship = Relationship::createModifiesRelationship(procedureReadYEntity, yEntity);
		Relationship assignRelation = Relationship::createModifiesRelationship(assignEntity, xEntity);
		Relationship readRelation = Relationship::createModifiesRelationship(readEntity, yEntity);

		vector <Relationship> expectedRelationships = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };

		vector<Pattern> expectedPatterns{ Pattern::createAssignPattern(1, " x ", " 1 ") };

		testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);

	}
	
	
	SECTION("Program with if-else and while statements") {
		/*
		procedure main {
			1. x = 3;
			2. read y;
			3. while ( x != 0 ) {
			4.     x = x - 1;
			5.     print y;
			}
			6. if (y > 5) then {
			7.     x = y;
			} else {
			8.     y = x;
			}
		}
		*/
	
		string program = "procedure main {\nx = 3;\nread y;\nwhile ( x != 0 ) {\nx = x - 1;\nprint y;\n}\nif (y > 5) then {\nx = y;\n} else {\ny = x;\n}\n}";
	
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
		shared_ptr<ASTNode> assign1Node = AssignASTNode::createAssignNode();
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
		shared_ptr<ASTNode> assign4Node = AssignASTNode::createAssignNode();
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
		Entity constantThreeEntity = Entity::createConstantEntity(constThreeNode->getLineNumber(), constThreeToken);
		Entity constantFiveEntity = Entity::createConstantEntity(constFiveNode->getLineNumber(), constFiveToken);
		Entity constantZeroEntity = Entity::createConstantEntity(constZeroNode->getLineNumber(), constZeroToken);
		Entity constantOneEntity = Entity::createConstantEntity(constOneNode->getLineNumber(), constOneToken);
		Entity y2Entity = Entity::createVariableEntity(y2Node->getLineNumber(), yToken);
		Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

		Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());
		Entity notEqualEntity = Entity::createExpressionEntity(notEqualNode->getLineNumber(), notEqualToken);

		Entity assign4Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
		Entity x3Entity = Entity::createVariableEntity(x3Node->getLineNumber(), xToken);
		Entity x4Entity = Entity::createVariableEntity(x4RhsNode->getLineNumber(), xToken);
		Entity minusEntity = Entity::createExpressionEntity(minusNode->getLineNumber(), minusToken);
		Entity greaterEntity = Entity::createExpressionEntity(greaterNode->getLineNumber(), greaterToken);
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

		vector<Entity> expectedEntities = vector<Entity>{ procedureEntity, assign1Entity, x1Entity, y2Entity, readEntity, whileEntity,
														assign4Entity, x3Entity, x4Entity, x4Entity, printEntity, y5Entity, y6Entity,
														ifEntity, assign7Entity, x7Entity, y7Entity, assign8Entity, y8Entity, x8Entity,
														constantThreeEntity, constantFiveEntity, constantZeroEntity, constantOneEntity, notEqualEntity,
														greaterEntity, minusEntity };

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

		vector<Relationship> expectedRelationships = vector<Relationship>{ mainModifiesX1, mainModifiesY2, mainModifiesX4, mainModifiesX7,
																	mainModifiesY8, assign1ModifiesX1, readModifiesX4, whileModifiesX4, assign4ModifiesX4,
																	ifModifiesX7, ifModifiesY8, assign7ModifiesX7, assign8ModifiesY8, whileParentAssign4,
																	whileParentPrint, ifParentAssign7, elseParentAssign8, whileParentTAssign4,
																	whileParentTPrint, ifParentTAssign7, elseParentTAssign8, whileUsesX3,
																	whileUsesX4, whileUsesY5, assign4UsesX, printUsesY, ifUsesY6, ifUsesY7, ifUsesX8, assign7UsesY7,
																	assign8UsesX8, procUsesX3, procUsesX4, procUsesY5, procUsesY6, procUsesY7, procUsesX8,
																	assign1FollowsRead, readFollowsWhile, whileFollowsIf, assign4FollowsPrint, assign1FollowsTRead,
																	readFollowsTWhile, whileFollowsTIf, assign4FollowsTPrint, assign1FollowsTWhile, assign1FollowsTIf,
																	readFollowsTIf };

		vector<Pattern> expectedPattern = vector<Pattern>{
			Pattern::createAssignPattern(1, " x ", " 3 "),
			Pattern::createWhilePattern(3, "x"), // this is broken, TODO
			Pattern::createAssignPattern(4, " x ", " x 1 - "),
			Pattern::createIfPattern(6, "y"), // this is broken, TODO
			Pattern::createAssignPattern(7, " x ", " y "),
			Pattern::createAssignPattern(8, " y ", " x ")
		};
		testExtract(program, expectedEntities, expectedRelationships, expectedPattern);
	}
	
	SECTION("Program with call statements") {
		/*
		procedure main {
			1. call alpha;
		}

		procedure alpha {
			2. read y;
			3. if (y > 5) then {
			4.     call beta;
			} else {
			5.     print y;
			}
		}

		procedure beta {
			6. y = y + 3;
		}
		*/
	}
	
	string program = "procedure main {\ncall alpha;\n}\n procedure alpha {\nread y;\nif ( y > 5) then {\ncall beta;\n} else {\nprint y;\n}\n}\n procedure beta {\n y = y + 3;\n}";

	// Creating tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token alphaToken = Token::createNameOrKeywordToken("alpha");
	Token betaToken = Token::createNameOrKeywordToken("beta");

	Token callToken = Token::createCallToken();
	Token readToken = Token::createReadToken();
	Token ifToken = Token::createIfToken();
	Token printToken = Token::createPrintToken();
	Token assignToken = Token::createEqualsToken();

	Token greaterToken = Token::createGreaterThanToken();
	Token plusToken = Token::createPlusToken();
	Token yToken = Token::createNameOrKeywordToken("y");
	Token constThreeToken = Token::createIntegerToken("3");
	Token constFiveToken = Token::createIntegerToken("5");
	Token stmtListToken = Token::createPlaceholderToken();

	// Create entities
	Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
	Entity procedureAlphaEntity = Entity::createProcedureEntity(alphaToken);
	Entity procedureBetaEntity = Entity::createProcedureEntity(betaToken);

	Entity callAlphaEntity = Entity::createCallEntity(1);
	Entity readYEntity = Entity::createReadEntity(2);
	Entity ifEntity = Entity::createIfEntity(3);
	Entity ifCondEntity = Entity::createExpressionEntity(3, greaterToken);
	Entity callBetaEntity = Entity::createCallEntity(4);
	Entity printYEntity = Entity::createPrintEntity(5);
	Entity assignEntity = Entity::createAssignEntity(6);
	Entity plusEntity = Entity::createExpressionEntity(6, plusToken);

	Entity y2Entity = Entity::createVariableEntity(2, yToken);
	Entity y3Entity = Entity::createVariableEntity(3, yToken);
	Entity fiveConstEntity = Entity::createConstantEntity(3, constFiveToken);
	Entity threeConstEntity = Entity::createConstantEntity(6, constThreeToken);
	Entity y5Entity = Entity::createVariableEntity(5, yToken);
	Entity y6LhsEntity = Entity::createVariableEntity(6, yToken);
	Entity y6RhsEntity = Entity::createVariableEntity(6, yToken);

	vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, callAlphaEntity, procedureAlphaEntity, procedureAlphaEntity,
			readYEntity, y2Entity, ifEntity, ifCondEntity, y3Entity, fiveConstEntity, callBetaEntity, procedureBetaEntity, printYEntity, 
			y5Entity, procedureBetaEntity, assignEntity, y6LhsEntity, y6RhsEntity, plusEntity, threeConstEntity };

	// Create modifies relationships
	Relationship readModifiesY = Relationship::createModifiesRelationship(readYEntity, y2Entity);
	Relationship alphaModifiesY2 = Relationship::createModifiesRelationship(procedureAlphaEntity, y2Entity);
	Relationship alphaModifiesY6 = Relationship::createModifiesRelationship(procedureAlphaEntity, y6LhsEntity);
	Relationship assignModifiesY = Relationship::createModifiesRelationship(assignEntity, y6LhsEntity);
	Relationship betaModifiesY = Relationship::createModifiesRelationship(procedureBetaEntity, y6LhsEntity);
	Relationship callAlphaModifiesY2 = Relationship::createModifiesRelationship(callAlphaEntity, y2Entity);
	Relationship callAlphaModifiesY6 = Relationship::createModifiesRelationship(callAlphaEntity, y6LhsEntity);
	Relationship callBetaModifiesY = Relationship::createModifiesRelationship(callBetaEntity, y6LhsEntity);
	Relationship mainModfiesY2 = Relationship::createModifiesRelationship(procedureMainEntity, y2Entity);
	Relationship mainModfiesY6 = Relationship::createModifiesRelationship(procedureMainEntity, y6LhsEntity);
	Relationship ifModifiesY6 = Relationship::createModifiesRelationship(ifEntity, y6LhsEntity);


	// Create parent relationships
	Relationship ifParentCall = Relationship::createParentRelationship(ifEntity, callBetaEntity);
	Relationship ifParentPrint = Relationship::createParentRelationship(ifEntity, printYEntity);

	// Create parentT relationships
	Relationship ifParentTCall = Relationship::createParentTRelationship(ifEntity, callBetaEntity);
	Relationship ifParentTPrint = Relationship::createParentTRelationship(ifEntity, printYEntity);

	// Create uses relationships
	Relationship ifUsesY3 = Relationship::createUsesRelationship(ifEntity, y3Entity);
	Relationship ifUsesY5 = Relationship::createUsesRelationship(ifEntity, y5Entity);
	Relationship printYUsesY5 = Relationship::createUsesRelationship(printYEntity, y5Entity);
	Relationship alphaUsesY3 = Relationship::createUsesRelationship(procedureAlphaEntity, y3Entity);
	Relationship alphaUsesY5 = Relationship::createUsesRelationship(procedureAlphaEntity, y5Entity);
	Relationship assignUsesY6 = Relationship::createUsesRelationship(assignEntity, y6RhsEntity);
	Relationship betaUsesY6 = Relationship::createUsesRelationship(procedureBetaEntity, y6RhsEntity);

	Relationship mainUsesY3 = Relationship::createUsesRelationship(procedureMainEntity, y3Entity);
	Relationship mainUsesY6 = Relationship::createUsesRelationship(procedureMainEntity, y6RhsEntity);
	Relationship mainUsesY5 = Relationship::createUsesRelationship(procedureMainEntity, y5Entity);
	Relationship callAlphaUsesY3 = Relationship::createUsesRelationship(callAlphaEntity, y3Entity);
	Relationship callAlphaUsesY6 = Relationship::createUsesRelationship(callAlphaEntity, y6RhsEntity);
	Relationship callAlphaUsesY5 = Relationship::createUsesRelationship(callAlphaEntity, y5Entity);
	Relationship alphaUsesY6 = Relationship::createUsesRelationship(procedureAlphaEntity, y6RhsEntity);
	Relationship ifUsesY6 = Relationship::createUsesRelationship(ifEntity, y6RhsEntity);
	Relationship callBetaUsesY6 = Relationship::createUsesRelationship(callBetaEntity, y6RhsEntity);

	// create Follows relationship
	Relationship ifFollowsReadY = Relationship::createFollowsRelationship(readYEntity, ifEntity);

	// create FollowsT relationship
	Relationship ifFollowsTReadY = Relationship::createFollowsTRelationship(readYEntity, ifEntity);

	// create Calls relationship
	Relationship mainCallsAlpha = Relationship::createCallsRelationship(procedureMainEntity, procedureAlphaEntity);
	Relationship alphaCallsBeta = Relationship::createCallsRelationship(procedureAlphaEntity, procedureBetaEntity);

	// create CallsT relationship
	Relationship mainCallsTAlpha = Relationship::createCallsTRelationship(procedureMainEntity, procedureAlphaEntity);
	Relationship alphaCallsTBeta = Relationship::createCallsTRelationship(procedureAlphaEntity, procedureBetaEntity);
	Relationship mainCallsTBeta = Relationship::createCallsTRelationship(procedureMainEntity, procedureBetaEntity);

	// create CallsAttribute Relationship
	Relationship callAttribute1Alpha = Relationship::createCallStmtAttributeRelationship(callAlphaEntity, procedureAlphaEntity);
	Relationship callAttribute4Beta = Relationship::createCallStmtAttributeRelationship(callBetaEntity, procedureBetaEntity);

	vector<Relationship> expectedRelationships = vector<Relationship>{ readModifiesY, alphaModifiesY2, assignModifiesY, betaModifiesY, callAlphaModifiesY2, callBetaModifiesY,
																	alphaModifiesY6, callAlphaModifiesY6, mainModfiesY2, mainModfiesY6, ifModifiesY6,
																	ifParentCall, ifParentPrint, ifParentTCall, ifParentTPrint, ifUsesY3, 
																	ifUsesY5, printYUsesY5, alphaUsesY3, alphaUsesY5, assignUsesY6, 
																	betaUsesY6, ifFollowsReadY, ifFollowsTReadY, mainCallsAlpha, 
																	alphaCallsBeta, mainCallsTAlpha, alphaCallsTBeta, mainCallsTBeta,
																	mainUsesY3, mainUsesY6, mainUsesY5, callAlphaUsesY3, callAlphaUsesY6, callAlphaUsesY5,
																	alphaUsesY6, ifUsesY6, callBetaUsesY6, callAttribute1Alpha, callAttribute4Beta };


	vector<Pattern> expectedPatterns = vector<Pattern>{
			Pattern::createIfPattern(3, "y"),
			Pattern::createAssignPattern(6, " y ", " y 3 + ")};

	testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);
}

// Util
bool compareEntity(Entity entityOne, Entity entityTwo) {
	if (entityOne.getLine() == entityTwo.getLine()) {
		return entityOne.getString() > entityTwo.getString();
	}
	else {
		return entityOne.getLine() > entityTwo.getLine();
	}
};

bool comparePattern(Pattern patternOne, Pattern patternTwo) {
	if (patternOne.getLine() == patternTwo.getLine()) {
		return patternOne.getRhs() > patternTwo.getRhs();
	}
	else {
		return patternOne.getLine() > patternTwo.getLine();
	}
}

bool compareRelationship(Relationship relationshipOne, Relationship relationshipTwo) {
	if (relationshipOne.getType() == relationshipTwo.getType()) {
		return relationshipOne.getRhs().getLine() > relationshipTwo.getRhs().getLine();
	}
	return relationshipOne.getType() > relationshipTwo.getType();
}

TEST_CASE("Test Source Processor : extractEntity") {
	auto test = [](string sourceProgram, vector<Entity> expectedEntites) {
		stringstream ss(sourceProgram);

		SourceProcessor sp = SourceProcessor();
		sp.initialize(ss);

		vector<Entity> entities = sp.extractEntities();

		sort(entities.begin(), entities.end(), compareEntity);

		REQUIRE(entities.size() == expectedEntites.size());

		for (int i = 0; i < entities.size(); i++) {
			Entity expectedEntity = entities[i];
			bool check = find(expectedEntites.begin(), expectedEntites.end(), expectedEntity) != expectedEntites.end();
			REQUIRE(check);
		}
	};

	SECTION("Test Milestone 1 program") {
		/*
			procedure main {
			read x;
			print x;
			y = 0;
			if ((!(x == 0)) && (y < 1)) then {
				while (y >= 0) {
					x = x + 10;
					y = y - 10;
				}
			} else {
				y = x % 2 + y;
				z = 5 * x - y;
				z = z / 1 * 3;
			}
		}
		*/

		string program = "procedure main{\n\tread x; \n\tprint x; \n\ty = 0; \n    if ((!(x == 0)) && (y < 1)) then {\n\t\twhile(y >= 0) { \n\t\t\tx = x + 10; \n\t\t\ty = y - 10; \n\t\t } \n\t} else { \n\t\ty = x % 2 + y; \n\t\tz = 5 * x - y; \n\t\tz = z / 1 * 3; \n\t }\n}";

		vector<Entity> expected{
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("main")),

			Entity::createReadEntity(1),
			Entity::createVariableEntity(1, Token::createNameOrKeywordToken("x")),

			Entity::createPrintEntity(2),
			Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x")),

			Entity::createAssignEntity(3),
			Entity::createVariableEntity(3, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(3, Token::createIntegerToken("0")),

			Entity::createIfEntity(4),
			Entity::createExpressionEntity(4, Token::createAndToken()),

			Entity::createExpressionEntity(4, Token::createEqualityToken()),
			Entity::createVariableEntity(4, Token::createNameOrKeywordToken("x")),
			Entity::createConstantEntity(4, Token::createIntegerToken("0")),

			Entity::createExpressionEntity(4, Token::createLessThanToken()),
			Entity::createVariableEntity(4, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(4, Token::createIntegerToken("1")),

			Entity::createWhileEntity(5),
			Entity::createExpressionEntity(5, Token::createGreaterThanEqualToken()),
			Entity::createVariableEntity(5, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(5, Token::createIntegerToken("0")),

			Entity::createAssignEntity(6),
			Entity::createVariableEntity(6, Token::createNameOrKeywordToken("x")),
			Entity::createExpressionEntity(6, Token::createPlusToken()),
			Entity::createVariableEntity(6, Token::createNameOrKeywordToken("x")),
			Entity::createConstantEntity(6, Token::createIntegerToken("10")),

			Entity::createAssignEntity(7),
			Entity::createVariableEntity(7, Token::createNameOrKeywordToken("y")),
			Entity::createExpressionEntity(7, Token::createMinusToken()),
			Entity::createVariableEntity(7, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(7, Token::createIntegerToken("10")),

			Entity::createAssignEntity(8),
			Entity::createVariableEntity(8, Token::createNameOrKeywordToken("y")),
			Entity::createExpressionEntity(8, Token::createModulusToken()),
			Entity::createVariableEntity(8, Token::createNameOrKeywordToken("x")),
			Entity::createExpressionEntity(8, Token::createPlusToken()),
			Entity::createConstantEntity(8, Token::createIntegerToken("2")),
			Entity::createVariableEntity(8, Token::createNameOrKeywordToken("y")),

			Entity::createAssignEntity(9),
			Entity::createVariableEntity(9, Token::createNameOrKeywordToken("z")),
			Entity::createExpressionEntity(9, Token::createMultiplyToken()),
			Entity::createConstantEntity(9, Token::createIntegerToken("5")),
			Entity::createExpressionEntity(9, Token::createMinusToken()),
			Entity::createVariableEntity(9, Token::createNameOrKeywordToken("x")),
			Entity::createVariableEntity(9, Token::createNameOrKeywordToken("y")),


			Entity::createAssignEntity(10),
			Entity::createVariableEntity(10, Token::createNameOrKeywordToken("z")),
			Entity::createExpressionEntity(10, Token::createDivideToken()),
			Entity::createVariableEntity(10, Token::createNameOrKeywordToken("z")),
			Entity::createExpressionEntity(10, Token::createMultiplyToken()),
			Entity::createConstantEntity(10, Token::createIntegerToken("1")),
			Entity::createConstantEntity(10, Token::createIntegerToken("3")),
		};

		sort(expected.begin(), expected.end(), compareEntity);

		test(program, expected);
	}


	SECTION("Milestone 1 Program #2") {
		string program = "procedure main {\n     while (! ((1==2) && (3==4)))  {\n\t\tread x;\n\t }\n\t y = 5;\n\t }";
		vector<Entity> expected{
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("main")),

			Entity::createWhileEntity(1),
			Entity::createExpressionEntity(1, Token::createAndToken()),
			Entity::createExpressionEntity(1, Token::createEqualityToken()),
			Entity::createExpressionEntity(1, Token::createEqualityToken()),
			Entity::createConstantEntity(1, Token::createIntegerToken("1")),
			Entity::createConstantEntity(1, Token::createIntegerToken("2")),
			Entity::createConstantEntity(1, Token::createIntegerToken("3")),
			Entity::createConstantEntity(1, Token::createIntegerToken("4")),
			Entity::createReadEntity(2),
			Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x")),
			Entity::createAssignEntity(3),
			Entity::createVariableEntity(3, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(3, Token::createIntegerToken("5")),
		};
		test(program, expected);
	}

	SECTION("Milestone 1 Program #3") {
		string program = "procedure main {\n     while (1>= 1%((0-1)) )  {\n\t\tread x;\n\t }\n\t x = 4;\n\t }";
		vector<Entity> expected{
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("main")),

			Entity::createWhileEntity(1),

			Entity::createExpressionEntity(1, Token::createGreaterThanEqualToken()),
			Entity::createExpressionEntity(1, Token::createModulusToken()),
			Entity::createExpressionEntity(1, Token::createMinusToken()),
			Entity::createConstantEntity(1, Token::createIntegerToken("0")),
			Entity::createConstantEntity(1, Token::createIntegerToken("1")),
			Entity::createConstantEntity(1, Token::createIntegerToken("1")),
			Entity::createConstantEntity(1, Token::createIntegerToken("1")),

			Entity::createReadEntity(2),
			Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x")),
			Entity::createAssignEntity(3),
			Entity::createVariableEntity(3, Token::createNameOrKeywordToken("x")),
			Entity::createConstantEntity(3, Token::createIntegerToken("4")),
		};
		test(program, expected);
	}

	SECTION("Milestone 1 Program #4") {
		string program = "procedure main {\n     while (1>= 1%((1)) )  {\n\t\tread x;\n\t }\n\t x = 4;\n\t }";
		vector<Entity> expected{
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("main")),

			Entity::createWhileEntity(1),

			Entity::createExpressionEntity(1, Token::createGreaterThanEqualToken()),
			Entity::createExpressionEntity(1, Token::createModulusToken()),
			Entity::createConstantEntity(1, Token::createIntegerToken("1")),
			Entity::createConstantEntity(1, Token::createIntegerToken("1")),
			Entity::createConstantEntity(1, Token::createIntegerToken("1")),

			Entity::createReadEntity(2),
			Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x")),
			Entity::createAssignEntity(3),
			Entity::createVariableEntity(3, Token::createNameOrKeywordToken("x")),
			Entity::createConstantEntity(3, Token::createIntegerToken("4")),
		};
		test(program, expected);
	}
}

TEST_CASE("Test Source Processor : extractPattern") {
	auto test = [](string sourceProgram, vector<Pattern> expectedPatterns) {
		stringstream ss(sourceProgram);

		SourceProcessor sp = SourceProcessor();
		sp.initialize(ss);

		vector<Pattern> patterns = sp.extractPatterns();

		sort(patterns.begin(), patterns.end(), comparePattern);

		REQUIRE(patterns.size() == expectedPatterns.size());

		for (int i = 0; i < patterns.size(); i++) {
			Pattern patternExtracted = patterns[i];
			Pattern patternExpected = expectedPatterns[i];
			bool check = patterns[i].equals(expectedPatterns[i]);
			REQUIRE(patterns[i].equals(expectedPatterns[i]));
		}
	};

	SECTION("Milestone 1 Program #2") {
		string program = "procedure main {\n     while (! ((1==2) && (3==4)))  {\n\t\tread x;\n\t }\n\t y = y + 5;\n\t }";
		vector<Pattern> expected{
			Pattern::createAssignPattern(3," y "," y 5 + "),
		};
		test(program, expected);
	}

	SECTION("Milestone 1 Program #3") {
		string program = "procedure main {\n     while (1>= 1%((0-1)) )  {\n\t\tread x;\n\t }\n\t x = x + 1;\n\t }";
		vector<Pattern> expected{
			Pattern::createAssignPattern(3, " x "," x 1 + "),
		};
		test(program, expected);
	}

	SECTION("Milestone 1 Program #4") {
		string program = "procedure main {\n     while (1>= 1%((1)) )  {\n\t\tread x;\n\t }\n\t x = x + 1;\n\t }";
		vector<Pattern> expected{
			Pattern::createAssignPattern(3, " x "," x 1 + "),
		};
		test(program, expected);
	}

	SECTION("Test Milestone 1 program") {
		/*
			procedure main {
			read x;
			print x;
			y = 0;
			if ((!(x == 0)) && (y < 1)) then {
				while (y >= 0) {
					x = x + 10;
					y = y - 10;
				}
			} else {
				y = x % 2 + y;
				z = 5 * x - y;
				z = z / 1 * 3;
			}
		}
		*/

		string program = "procedure main{\n\tread x; \n\tprint x; \n\ty = 0; \n    if ((!(x == 0)) && (y < 1)) then {\n\t\twhile(y >= 0) { \n\t\t\tx = x + 10; \n\t\t\ty = y - 10; \n\t\t } \n\t} else { \n\t\ty = x % 2 + y; \n\t\tz = 5 * x - y; \n\t\tz = z / 1 * 3; \n\t }\n}";

		vector<Pattern> expected{
			Pattern::createAssignPattern(3," y ", " 0 "),
			Pattern::createIfPattern(4,"x y"),
			Pattern::createWhilePattern(5,"y"),
			Pattern::createAssignPattern(6," x ", " x 10 + "),
			Pattern::createAssignPattern(7," y ", " y 10 - "),
			Pattern::createAssignPattern(8," y ", " x 2 % y + "),
			Pattern::createAssignPattern(9," z ", " 5 x * y - "),
			Pattern::createAssignPattern(10," z ", " z 1 / 3 * "),
		};

		sort(expected.begin(), expected.end(), comparePattern);

		test(program, expected);
	}

	SECTION("Test milestone 1 program #2") {
		/*
			procedure main {
			  read x;
			  print x;
			  y = 0;
			  if ((!(d == 0)) && (y < 1)) then {
				 while (e >= 0) {
					x = x + 10 * 2;
					y = y - 10;
					b = a;
				 } 
			  } else {
				 y = x % 2 + y;
				 z = 5 * x - y;
				 z = z / 1 * 3;
				 print c;
			  }
			  x = (x + 1) - 1;
			}
		*/
		string program = "procedure main {\n  read x;\n  print x;\n  y = 0;\n  if ((!(d == 0)) && (y < 1)) then {\n     while (e >= 0) {\n        x = x + 10 * 2;\n        y = y - 10;\n        b = a;\n     } \n  } else {\n     y = x % 2 + y;\n     z = 5 * x - y;\n     z = z / 1 * 3;\n     print c;\n  }\n  x = (x + 1) - 1;\n  }";

		vector<Pattern> expected{
			Pattern::createAssignPattern(3," y ", " 0 "),
			Pattern::createIfPattern(4,"d y"),
			Pattern::createWhilePattern(5,"e"),
			Pattern::createAssignPattern(6," x ", " x 10 2 * + "),
			Pattern::createAssignPattern(7," y ", " y 10 - "),
			Pattern::createAssignPattern(8," b ", " a "),
			Pattern::createAssignPattern(9," y ", " x 2 % y + "),
			Pattern::createAssignPattern(10," z ", " 5 x * y - "),
			Pattern::createAssignPattern(11," z ", " z 1 / 3 * "),
			Pattern::createAssignPattern(13," x ", " x 1 + 1 - "),
		};

		sort(expected.begin(), expected.end(), comparePattern);

		test(program, expected);
	}

}

TEST_CASE("Test Source Processor : extractRelations") {
	auto test = [](string sourceProgram, vector<Relationship> expectedRelations) {
		stringstream ss(sourceProgram);

		SourceProcessor sp = SourceProcessor();
		sp.initialize(ss);

		vector<Relationship> relations = sp.extractRelations();

		sort(relations.begin(), relations.end(), compareRelationship);

		REQUIRE(relations.size() == expectedRelations.size());

		for (int i = 0; i < expectedRelations.size(); i++) {
			Relationship expectedRelation = expectedRelations[i];
			bool check = find(relations.begin(), relations.end(), expectedRelation) != relations.end();
			if (!check) {
				int x = 1;
			}
			REQUIRE(find(relations.begin(), relations.end(), expectedRelation) != relations.end());
		}
	};

	SECTION("Test Milestone 1 program") {
		/*
			procedure main {
			read x;
			print x;
			y = 0;
			if ((!(x == 0)) && (y < 1)) then {
				while (y >= 0) {
					x = x + 10;
					y = y - 10;
				}
			} else {
				y = x % 2 + y;
				z = 5 * x - y;
				z = z / 1 * 3;
			}
		}
		*/

		string program = "procedure main{\n\tread x; \n\tprint x; \n\ty = 0; \n    if ((!(x == 0)) && (y < 1)) then {\n\t\twhile(y >= 0) { \n\t\t\tx = x + 10; \n\t\t\ty = y - 10; \n\t\t } \n\t} else { \n\t\ty = x % 2 + y; \n\t\tz = 5 * x - y; \n\t\tz = z / 1 * 3; \n\t }\n}";

		Token procedureToken = Token::createNameOrKeywordToken("main");
		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token zToken = Token::createNameOrKeywordToken("z");
		Token zeroToken = Token::createIntegerToken("0");
		Token oneToken = Token::createIntegerToken("1");
		Token twoToken = Token::createIntegerToken("2");
		Token threeToken = Token::createIntegerToken("3");
		Token fiveToken = Token::createIntegerToken("5");
		Token tenToken = Token::createIntegerToken("10");

		vector<Relationship> expectedFollows {
			Relationship::createFollowsRelationship(Entity::createReadEntity(1), Entity::createPrintEntity(2)),
			Relationship::createFollowsRelationship(Entity::createPrintEntity(2), Entity::createAssignEntity(3)),
			Relationship::createFollowsRelationship(Entity::createAssignEntity(3), Entity::createIfEntity(4)),

			Relationship::createFollowsRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(7)),
			Relationship::createFollowsRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(9)),
			Relationship::createFollowsRelationship(Entity::createAssignEntity(9), Entity::createAssignEntity(10)),
		};
		vector<Relationship> expectedFollowT {
			Relationship::createFollowsTRelationship(Entity::createReadEntity(1), Entity::createPrintEntity(2)),
			Relationship::createFollowsTRelationship(Entity::createReadEntity(1), Entity::createAssignEntity(3)),
			Relationship::createFollowsTRelationship(Entity::createReadEntity(1), Entity::createIfEntity(4)),

			Relationship::createFollowsTRelationship(Entity::createPrintEntity(2), Entity::createAssignEntity(3)),
			Relationship::createFollowsTRelationship(Entity::createPrintEntity(2), Entity::createIfEntity(4)),

			Relationship::createFollowsTRelationship(Entity::createAssignEntity(3), Entity::createIfEntity(4)),

			Relationship::createFollowsTRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(7)),
			Relationship::createFollowsTRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(9)),
			Relationship::createFollowsTRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(10)),
			Relationship::createFollowsTRelationship(Entity::createAssignEntity(9), Entity::createAssignEntity(10)),
		};

		vector<Relationship> expectedParent {
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createWhileEntity(5)),
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(8)),
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(9)),
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(10)),

			Relationship::createParentRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(6)),
			Relationship::createParentRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(7)),
		};

		vector<Relationship> expectedParentT {
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createWhileEntity(5)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(6)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(7)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(8)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(9)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(10)),

			Relationship::createParentTRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(6)),
			Relationship::createParentTRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(7)),
		};



		vector<Relationship> expectedModifies{
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(1, xToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(3, yToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(7, yToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(8, yToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(9, zToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(10, zToken)),

			Relationship::createModifiesRelationship(Entity::createReadEntity(1), Entity::createVariableEntity(1, xToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(3), Entity::createVariableEntity(3, yToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(6), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(7), Entity::createVariableEntity(7, yToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(8), Entity::createVariableEntity(8, yToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(9), Entity::createVariableEntity(9, zToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(10), Entity::createVariableEntity(10, zToken)),

			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(7, yToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(8, yToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(9, zToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(10, zToken)),

			Relationship::createModifiesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(7, yToken)),

		};

		vector<Relationship> expectedUses{
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(2, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(4, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(4, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(5, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(6, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(7, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(8, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(8, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(9, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(9, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(10, zToken)),

			Relationship::createUsesRelationship(Entity::createPrintEntity(2), Entity::createVariableEntity(2, xToken)),

			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(4, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(4, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(5, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(6, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(7, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(8, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(8, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(9, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(9, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(10, zToken)),

			Relationship::createUsesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(5, yToken)),
			Relationship::createUsesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(6, xToken)),	
			Relationship::createUsesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(7, yToken)),
			Relationship::createUsesRelationship(Entity::createAssignEntity(6), Entity::createVariableEntity(6, xToken)),
			Relationship::createUsesRelationship(Entity::createAssignEntity(7), Entity::createVariableEntity(7, yToken)),

			Relationship::createUsesRelationship(Entity::createAssignEntity(8), Entity::createVariableEntity(8, xToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(8), Entity::createVariableEntity(8, yToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(9), Entity::createVariableEntity(9, xToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(9), Entity::createVariableEntity(9, yToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(10), Entity::createVariableEntity(10, zToken)),	
		};

		vector<Relationship> expected;
		expected.insert(expected.end(), expectedFollows.begin(), expectedFollows.end());
		expected.insert(expected.end(), expectedFollowT.begin(), expectedFollowT.end());
		expected.insert(expected.end(), expectedParent.begin(), expectedParent.end());
		expected.insert(expected.end(), expectedParentT.begin(), expectedParentT.end());
		expected.insert(expected.end(), expectedModifies.begin(), expectedModifies.end());
		expected.insert(expected.end(), expectedUses.begin(), expectedUses.end());

		sort(expected.begin(), expected.end(), compareRelationship);

		test(program, expected);
	}
}

TEST_CASE("Test extractCFGRelations") {
	auto test = [](string sourceProgram, vector<Relationship> expectedRelations) {
		stringstream ss(sourceProgram);

		SourceProcessor sp = SourceProcessor();
		sp.initialize(ss);

		vector<Relationship> relations = sp.extractCFGRelations();

		if (relations.size() != expectedRelations.size()) {
			int x = 1;
		}

		REQUIRE(relations.size() == expectedRelations.size());

		for (int i = 0; i < expectedRelations.size(); i++) {
			Relationship expectedRelation = expectedRelations[i];
			bool check = find(relations.begin(), relations.end(), expectedRelation) != relations.end();
			if (!check) {
				int x = 1;
			}
			REQUIRE(find(relations.begin(), relations.end(), expectedRelation) != relations.end());
		}
	};

	SECTION("Test program with 3 procedures") {
		/*
			procedure main1 {
			1.	while (1>= 1%((1))) {
			2.		read x;
				}
			3.	x = x + 1;
			}

			procedure main2 {
			4.	if (x >= (y - 10)) then {
			5.		read x;
				} else {
			6.		read y;
				}
			7.	print z;
			}

			procedure main3 {
			8.	x = 10;
			9.	if (x == 5) then {
			10.		while (x != 0) {
			11.			print z;
					}
				} else {
			12.		read z;
				}
			13.	call main2;
			}
			*/
		string program = "procedure main1 {\n\twhile (1>= 1%((1))) {\n\t\tread x;\n\t}\n\tx = x + 1;\n\t}\n\n\tprocedure main2 {\n\t\tif (x >= (y - 10)) then {\n\t\t\tread x;\n\t\t} else {\n\t\t\tread y;\n\t\t}\n\t\tprint z;\n\t}\n\n\tprocedure main3 {\n\t\tx = 10;\n\t\tif (x == 5) then {\n\t\t\twhile (x != 0) {\n\t\t\t\tprint z;\n\t\t\t}\n\t\t} else {\n\t\t\tread z;\n\t\t}\n\t\tcall main2;\n\t}\n";

		vector<Relationship> expectedRelations = {
			// main 1
			Relationship::createNextRelationship(Entity::createWhileEntity(1),Entity::createReadEntity(2)),
			Relationship::createNextRelationship(Entity::createWhileEntity(1),Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createReadEntity(2),Entity::createWhileEntity(1)),

			// main2
			Relationship::createNextRelationship(Entity::createIfEntity(4),Entity::createReadEntity(5)),
			Relationship::createNextRelationship(Entity::createIfEntity(4),Entity::createReadEntity(6)),
			Relationship::createNextRelationship(Entity::createReadEntity(5),Entity::createPrintEntity(7)),
			Relationship::createNextRelationship(Entity::createReadEntity(6),Entity::createPrintEntity(7)),

			// main3
			Relationship::createNextRelationship(Entity::createAssignEntity(8),Entity::createIfEntity(9)),
			Relationship::createNextRelationship(Entity::createIfEntity(9),Entity::createWhileEntity(10)),
			Relationship::createNextRelationship(Entity::createIfEntity(9),Entity::createReadEntity(12)),
			Relationship::createNextRelationship(Entity::createWhileEntity(10),Entity::createPrintEntity(11)),
			Relationship::createNextRelationship(Entity::createWhileEntity(10),Entity::createCallEntity(13)),
			Relationship::createNextRelationship(Entity::createPrintEntity(11),Entity::createWhileEntity(10)),
			Relationship::createNextRelationship(Entity::createReadEntity(12),Entity::createCallEntity(13)),
		};

		test(program, expectedRelations);
	}

	SECTION("Test nested while") {
		string program = "procedure Main { \
								while (iter <= 5) { \
									if (iter != 1) then { \
										iter = iter; \
									} else { \
										x = x; \
									} while (iter < 1) { \
										iter = iter; \
									} \
								} \
								x = x + center; \
						  }";

		vector<Relationship> expectedRelations = {
			// main 1
			Relationship::createNextRelationship(Entity::createWhileEntity(1),Entity::createIfEntity(2)),
			Relationship::createNextRelationship(Entity::createWhileEntity(1),Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createAssignEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createAssignEntity(4)),
			Relationship::createNextRelationship(Entity::createAssignEntity(3),Entity::createWhileEntity(5)),
			Relationship::createNextRelationship(Entity::createAssignEntity(4),Entity::createWhileEntity(5)),
			Relationship::createNextRelationship(Entity::createWhileEntity(5),Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createWhileEntity(5),Entity::createWhileEntity(1)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6),Entity::createWhileEntity(5)),
		};

		test(program, expectedRelations);
	}

	SECTION("Test multiple procedures") {
		string program = "procedure main { \
							x = 1; \
							read z; \
							if (x == 1) then { \
								z = 2; \
								while (y >= 0) { \
									x = x + 10; \
									y = y + 10; \
									b = 1; \
								} \
								a = 3; \
							} \
							else { \
								read x; \
								print z; \
								call test; \
							} \
							print x; \
							x = (x + 1) - 1; \
						} \
						procedure test { \
						  read y; \
						  while (y > 0) { \
							print y; \
							y = y - 1; \
						  } \
						}";

		vector<Relationship> expectedRelations = {
			// main
			Relationship::createNextRelationship(Entity::createAssignEntity(1),Entity::createReadEntity(2)),
			Relationship::createNextRelationship(Entity::createReadEntity(2),Entity::createIfEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createAssignEntity(4)),
			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createReadEntity(10)),

			Relationship::createNextRelationship(Entity::createAssignEntity(4),Entity::createWhileEntity(5)),
			Relationship::createNextRelationship(Entity::createWhileEntity(5),Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6),Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(7),Entity::createAssignEntity(8)),
			Relationship::createNextRelationship(Entity::createAssignEntity(8),Entity::createWhileEntity(5)),
			Relationship::createNextRelationship(Entity::createWhileEntity(5),Entity::createAssignEntity(9)),

			Relationship::createNextRelationship(Entity::createReadEntity(10),Entity::createPrintEntity(11)),
			Relationship::createNextRelationship(Entity::createPrintEntity(11),Entity::createCallEntity(12)),

			Relationship::createNextRelationship(Entity::createAssignEntity(9),Entity::createPrintEntity(13)),
			Relationship::createNextRelationship(Entity::createCallEntity(12),Entity::createPrintEntity(13)),

			Relationship::createNextRelationship(Entity::createPrintEntity(13),Entity::createAssignEntity(14)),

			// test
			Relationship::createNextRelationship(Entity::createReadEntity(15),Entity::createWhileEntity(16)),
			Relationship::createNextRelationship(Entity::createWhileEntity(16),Entity::createPrintEntity(17)),
			Relationship::createNextRelationship(Entity::createPrintEntity(17),Entity::createAssignEntity(18)),
			Relationship::createNextRelationship(Entity::createAssignEntity(18),Entity::createWhileEntity(16)),
		};

		test(program, expectedRelations);
	}

	SECTION("Test milestone 3 bug #1") {
		string program = "procedure x { \
			while (x == 1) { \
				if (x == 1) then{\
				  if (x == 1) then {\
					x = 1; }\
				  else {\
					x = 1;\
		   } }\
				else {\
					x = 1;\
				}\
				\
			x = 1;}}";

		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createWhileEntity(1),Entity::createIfEntity(2)),

			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createIfEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createAssignEntity(6)),

			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createAssignEntity(4)),
			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createAssignEntity(5)),

			Relationship::createNextRelationship(Entity::createAssignEntity(4),Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(5),Entity::createAssignEntity(7)),

			Relationship::createNextRelationship(Entity::createAssignEntity(6),Entity::createAssignEntity(7)),

			Relationship::createNextRelationship(Entity::createAssignEntity(7),Entity::createWhileEntity(1)),
		};

		test(program, expectedRelations);
	}

	SECTION("Test milestone 3 bug #2") {
		string program = "procedure x { \
			while (x == 1) { \
				if (x == 1) then{\
				  if (x == 1) then {\
					x = 1; }\
				  else {\
					x = 1;\
		   } x = 2;}\
				else {\
					x = 1;\
				}\
			}}";

		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createWhileEntity(1),Entity::createIfEntity(2)),

			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createIfEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createAssignEntity(4)),
			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createAssignEntity(5),Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createAssignEntity(4),Entity::createAssignEntity(6)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6),Entity::createWhileEntity(1)),

			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(7),Entity::createWhileEntity(1)),
		};

		test(program, expectedRelations);
	}

	SECTION("Test milestone 3 bug #3") {
		string program = "procedure x {\
			while (x == 1) {\
				if (x == 1) then{\
				  if (x == 1) then {\
					 if (x == 1) then {\
					  x = 1;\
					 }\
		   else {\
		  x = 1;\
		}\
	 }\
else {\
x = 1;\
}\
  x = 1;\
				}\
				else {\
					x = 1;\
				}\
			}\
	}\
	";

		vector<Relationship> expectedRelations = {
			Relationship::createNextRelationship(Entity::createWhileEntity(1),Entity::createIfEntity(2)),

			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createIfEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(2),Entity::createAssignEntity(9)),


			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createIfEntity(4)),
			Relationship::createNextRelationship(Entity::createIfEntity(3),Entity::createAssignEntity(7)),

			Relationship::createNextRelationship(Entity::createIfEntity(4),Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createIfEntity(4),Entity::createAssignEntity(6)),

			Relationship::createNextRelationship(Entity::createAssignEntity(5),Entity::createAssignEntity(8)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6),Entity::createAssignEntity(8)),
			Relationship::createNextRelationship(Entity::createAssignEntity(7),Entity::createAssignEntity(8)),

			Relationship::createNextRelationship(Entity::createAssignEntity(8),Entity::createWhileEntity(1)),
			Relationship::createNextRelationship(Entity::createAssignEntity(9),Entity::createWhileEntity(1)),
		};

		test(program, expectedRelations);
	}

	SECTION("Milestone 3 bug #4") {
		string program = "procedure as {\
			while (x == 1) {\
				while (x == 1) {\
					if (x == 1) then{\
					  if (x == 1) then {\
						x = 1;\
					  }\
					  else {\
						x = 1;\
					  }\
					  x = 1;\
					}\
					else {\
						x = 1;\
					}\
					x = 1;\
				}\
			}\
	}";
		vector<Relationship> expected = {
			Relationship::createNextRelationship(Entity::createWhileEntity(1), Entity::createWhileEntity(2)),

			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createIfEntity(3)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createWhileEntity(1)),

			Relationship::createNextRelationship(Entity::createIfEntity(3), Entity::createIfEntity(4)),
			Relationship::createNextRelationship(Entity::createIfEntity(3), Entity::createAssignEntity(8)),

			Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(6)),

			Relationship::createNextRelationship(Entity::createAssignEntity(5), Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(7)),

			Relationship::createNextRelationship(Entity::createAssignEntity(7), Entity::createAssignEntity(9)),

			Relationship::createNextRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(9)),
			Relationship::createNextRelationship(Entity::createAssignEntity(9), Entity::createWhileEntity(2)),
		};

		test(program, expected);
	}

	SECTION("Milestone 3 bug Extra testing") {
		string program = "procedure as {\
			while (x == 1) {\
				while (x == 1) {\
					if (x == 1) then{\
					  if (x == 1) then {\
						x = 1;\
						if (x == 1) then {\
						  x = 1;\
						}\
						else {\
							x = 1;\
						}\
					 }\
					 else {\
						x = 1;\
						while (x == 1) {\
							if (x == 1) then {\
								x = 1;\
							}\
							else {\
								x = 1;\
							}\
						}\
					}\
					x = 1;\
				  }\
				   else {\
					x = 1;\
				  }\
				  x = 1;\
				}\
			}\
	}";
		vector<Relationship> expected = {
			Relationship::createNextRelationship(Entity::createWhileEntity(1), Entity::createWhileEntity(2)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createIfEntity(3)),
			Relationship::createNextRelationship(Entity::createIfEntity(3), Entity::createIfEntity(4)),
			Relationship::createNextRelationship(Entity::createIfEntity(3), Entity::createAssignEntity(15)),
			Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(5)),
			Relationship::createNextRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(9)),
			Relationship::createNextRelationship(Entity::createAssignEntity(5), Entity::createIfEntity(6)),
			Relationship::createNextRelationship(Entity::createIfEntity(6), Entity::createAssignEntity(7)),
			Relationship::createNextRelationship(Entity::createIfEntity(6), Entity::createAssignEntity(8)),
			Relationship::createNextRelationship(Entity::createAssignEntity(7), Entity::createAssignEntity(14)),
			Relationship::createNextRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(14)),
			Relationship::createNextRelationship(Entity::createAssignEntity(14), Entity::createAssignEntity(16)),
			Relationship::createNextRelationship(Entity::createAssignEntity(16), Entity::createWhileEntity(2)),
			Relationship::createNextRelationship(Entity::createAssignEntity(9), Entity::createWhileEntity(10)),
			Relationship::createNextRelationship(Entity::createWhileEntity(10), Entity::createIfEntity(11)),
			Relationship::createNextRelationship(Entity::createIfEntity(11), Entity::createAssignEntity(12)),
			Relationship::createNextRelationship(Entity::createIfEntity(11), Entity::createAssignEntity(13)),
			Relationship::createNextRelationship(Entity::createAssignEntity(12), Entity::createWhileEntity(10)),
			Relationship::createNextRelationship(Entity::createAssignEntity(13), Entity::createWhileEntity(10)),
			Relationship::createNextRelationship(Entity::createWhileEntity(10), Entity::createAssignEntity(14)),
			Relationship::createNextRelationship(Entity::createAssignEntity(15), Entity::createAssignEntity(16)),
			Relationship::createNextRelationship(Entity::createWhileEntity(2), Entity::createWhileEntity(1)),
		};

		test(program, expected);
	}
}
