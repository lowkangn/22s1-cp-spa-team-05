#include "catch.hpp"

#include <sp/design_extractor/ParentExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>
#include <memory>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>

using namespace std;

TEST_CASE("ParentExtractor: test extractFromContainer") {
	auto test = [](shared_ptr<ASTNode> containerAstNode, Entity leftHandSide, vector<Relationship> expectedResult) {
		// Given
		ParentExtractor extractor = ParentExtractor();

		// When
		vector<Relationship> extractedRelationships = extractor.extractFromContainer(containerAstNode, leftHandSide);

		// Then
		REQUIRE(extractedRelationships.size() == expectedResult.size());

		for (int i = 0; i < extractedRelationships.size(); i++) {
			REQUIRE(extractedRelationships[i].equals(expectedResult[i]));
		}
	};

	SECTION("Stmt lst node") {
		/*
			{
				read x;
				x = 1;
				call proc;
				if () then {} else {}
				while () {}
			}
		*/

		Token readToken = Token{ READ_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token callToken = Token{ CALL_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token ifToken = Token{ IF_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token whileToken = Token{ WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD };

		Entity ifParentEntity = Entity::createIfEntity(1);

		// Stmtlst node
		shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(Token::getPlaceHolderToken()));

		// Add some statements

		shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
		readNode->setLineNumber(2);
		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		assignNode->setLineNumber(3);
		shared_ptr<ASTNode> callNode(new CallASTNode(Token::getPlaceHolderToken()));
		callNode->setLineNumber(4);


		// Doesnt have to be full implemented as extractFromContainer is shallow (extract method handles nested containers)
		shared_ptr<ASTNode> IfNode(new IfASTNode(ifToken));
		IfNode->setLineNumber(5);
		shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
		whileNode->setLineNumber(6);

		Entity readEntity = Entity::createReadEntity(2);
		Entity assignEntity = Entity::createAssignEntity(3);
		Entity callEntity = Entity::createCallEntity(4);
		Entity ifEntity = Entity::createIfEntity(5);
		Entity whileEntity = Entity::createWhileEntity(6);

		stmtLstNode->addChild(readNode);
		stmtLstNode->addChild(assignNode);
		stmtLstNode->addChild(callNode);
		stmtLstNode->addChild(IfNode);
		stmtLstNode->addChild(whileNode);

		Relationship readParent = Relationship(ifParentEntity, readEntity, RelationshipType::PARENT);
		Relationship assignParent = Relationship(ifParentEntity, assignEntity, RelationshipType::PARENT);
		Relationship callParent = Relationship(ifParentEntity, callEntity, RelationshipType::PARENT);
		Relationship ifParent = Relationship(ifParentEntity, ifEntity, RelationshipType::PARENT);
		Relationship whileParent = Relationship(ifParentEntity, whileEntity, RelationshipType::PARENT);

		vector<Relationship> expectedResult{ readParent , assignParent, callParent, ifParent, whileParent };

		test(stmtLstNode, ifParentEntity, expectedResult);
	}
}

TEST_CASE("ParentExtractor: test extract") {
	auto test = [](shared_ptr<ASTNode> astNode, vector<Relationship> expectedResult) {
		// Given
		ParentExtractor extractor = ParentExtractor();

		// When
		vector<Relationship> extractedRelationships = extractor.extract(astNode);

		// Then
		REQUIRE(extractedRelationships.size() == expectedResult.size());

		for (int i = 0; i < extractedRelationships.size(); i++) {
			bool check = extractedRelationships[i].equals(expectedResult[i]);
			REQUIRE(extractedRelationships[i].equals(expectedResult[i]));
		}
	};

	SECTION("Test nested") {
		/*
			procedure main {
			1.	while (x > 0) {
			2.		if (x > 0) then {
			3.			read x;
					} else {
			4.			read y;
					}
				}
			}
		*/

		Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
		Token readToken = Token{ READ_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token whileToken = Token{ WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token ifToken = Token{ IF_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token greaterThan = Token{ GREATER_OPERATOR, TokenType::OPERATOR };

		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));
		shared_ptr<ASTNode> mainStmtList(new StatementListASTnode(Token::getPlaceHolderToken()));
		procedureNode->addChild(mainStmtList);

		shared_ptr<ASTNode> xNode(new VariableASTNode(xToken));
		xNode->setLineNumber(3);
		shared_ptr<ASTNode> yNode(new VariableASTNode(yToken));
		yNode->setLineNumber(4);

		shared_ptr<ASTNode> readYNode(new ReadASTNode(readToken));
		readYNode->setLineNumber(4);

		shared_ptr<ASTNode> readXNode(new ReadASTNode(readToken));
		readXNode->setLineNumber(3);

		shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
		whileNode->setLineNumber(1);

		mainStmtList->addChild(whileNode);

		shared_ptr<ASTNode> whileStmtLstNode(new StatementListASTnode(Token::getPlaceHolderToken()));
		shared_ptr<ASTNode> whileCond(new ExpressionASTNode(greaterThan));

		whileNode->addChild(whileCond);
		whileNode->addChild(whileStmtLstNode);

		shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));
		shared_ptr<ASTNode> ifCond(new ExpressionASTNode(greaterThan));
		ifNode->setLineNumber(2);

		whileStmtLstNode->addChild(ifNode);

		shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTnode(Token::getPlaceHolderToken()));
		shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTnode(Token::getPlaceHolderToken()));

		ifNode->addChild(ifCond);
		ifNode->addChild(thenStmtLstNode);
		ifNode->addChild(elseStmtLstNode);

		thenStmtLstNode->addChild(readXNode);
		elseStmtLstNode->addChild(readYNode);

		// Create entities
		Entity whileEntity = Entity::createWhileEntity(1);
		Entity ifEntity = Entity::createIfEntity(2);
		Entity readXEntity = Entity::createReadEntity(3);
		Entity readYEntity = Entity::createReadEntity(4);

		// Creating Relationships
		Relationship whileParentIf = Relationship(whileEntity, ifEntity, RelationshipType::PARENT);
		Relationship ifParentX = Relationship(ifEntity, readXEntity, RelationshipType::PARENT);
		Relationship ifParentY = Relationship(ifEntity, readYEntity, RelationshipType::PARENT);

		vector<Relationship> expectedParentRelationships{ ifParentY, ifParentX, whileParentIf };

		test(procedureNode, expectedParentRelationships);
	}

	SECTION("Full test") {
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
		Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
		Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
		Token notEqualToken = Token{ NOT_EQUAL_OPERATOR, TokenType::OPERATOR };
		Token greaterToken = Token{ GREATER_OPERATOR, TokenType::OPERATOR };
		Token minusToken = Token{ MINUS_OPERATOR, TokenType::OPERATOR };
		Token constThreeToken = Token{ "3", TokenType::INTEGER };
		Token constZeroToken = Token{ "0", TokenType::INTEGER };
		Token constOneToken = Token{ "1", TokenType::INTEGER };
		Token constFiveToken = Token{ "5", TokenType::INTEGER };
		Token readToken = Token{ READ_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token whileToken = Token{ WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token printToken = Token{ PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token ifToken = Token{ IF_KEYWORD, TokenType::NAME_OR_KEYWORD };
		Token stmtListToken = Token{ "", TokenType::INVALID };

		// Creating AST nodes
		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

		// Line 1 (x = 3)
		shared_ptr<ASTNode> mainStmtList(new StatementListASTnode(stmtListToken));
		shared_ptr<ASTNode> x1Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> assign1Node(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> constThreeNode(new ConstantValueASTNode(constThreeToken));

		x1Node->setLineNumber(1);
		assign1Node->setLineNumber(1);
		constThreeNode->setLineNumber(1);

		// Line 2 (read y)
		shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));
		shared_ptr<ASTNode> y2Node(new VariableASTNode(yToken));

		readNode->setLineNumber(2);
		y2Node->setLineNumber(2);

		// Line 3 (while ( x != 0 ))
		shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
		shared_ptr<ASTNode> x3Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> constZeroNode(new ConstantValueASTNode(constZeroToken));
		shared_ptr<ASTNode> notEqualNode(new ExpressionASTNode(notEqualToken));

		whileNode->setLineNumber(3);
		x3Node->setLineNumber(3);
		constZeroNode->setLineNumber(3);
		notEqualNode->setLineNumber(3);

		// Line 4 (x = x - 1)
		shared_ptr<ASTNode> whileStmtList(new StatementListASTnode(stmtListToken));
		shared_ptr<ASTNode> x4LhsNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> x4RhsNode(new VariableASTNode(xToken));
		shared_ptr<ASTNode> assign4Node(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> minusNode(new ExpressionASTNode(minusToken));
		shared_ptr<ASTNode> constOneNode(new ConstantValueASTNode(constOneToken));

		x4LhsNode->setLineNumber(4);
		x4RhsNode->setLineNumber(4);
		assign4Node->setLineNumber(4);
		minusNode->setLineNumber(4);
		constOneNode->setLineNumber(4);

		// Line 5 (print y)
		shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));
		shared_ptr<ASTNode> y5Node(new VariableASTNode(yToken));

		printNode->setLineNumber(5);
		y5Node->setLineNumber(5);

		// Line 6 (if (y > 5) then)
		shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));
		shared_ptr<ASTNode> y6Node(new VariableASTNode(yToken));
		shared_ptr<ASTNode> greaterNode(new ExpressionASTNode(greaterToken));
		shared_ptr<ASTNode> constFiveNode(new ConstantValueASTNode(constFiveToken));

		ifNode->setLineNumber(6);
		y6Node->setLineNumber(6);
		greaterNode->setLineNumber(6);
		constFiveNode->setLineNumber(6);

		// Line 7 (x = y)
		shared_ptr<ASTNode> thenStmtList(new StatementListASTnode(stmtListToken));
		shared_ptr<ASTNode> assign7Node(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> x7Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> y7Node(new VariableASTNode(yToken));

		assign7Node->setLineNumber(7);
		x7Node->setLineNumber(7);
		y7Node->setLineNumber(7);

		// Line 8 (y = x)
		shared_ptr<ASTNode> elseStmtList(new StatementListASTnode(stmtListToken));
		shared_ptr<ASTNode> assign8Node(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> y8Node(new VariableASTNode(yToken));
		shared_ptr<ASTNode> x8Node(new VariableASTNode(xToken));

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
		Entity x4Entity = Entity::createVariableEntity(x4RhsNode->getLineNumber(), xToken);
		Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());
		Entity y5Entity = Entity::createVariableEntity(y5Node->getLineNumber(), yToken);

		Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());

		Entity assign7Entity = Entity::createAssignEntity(assign7Node->getLineNumber());
		Entity x7Entity = Entity::createVariableEntity(x7Node->getLineNumber(), xToken);

		Entity assign8Entity = Entity::createAssignEntity(assign8Node->getLineNumber());
		Entity y8Entity = Entity::createVariableEntity(y8Node->getLineNumber(), yToken);

		// Creating Relationships
		Relationship whileParentX = Relationship(whileEntity, assign4Entity, RelationshipType::PARENT);
		Relationship whileParentPrint = Relationship(whileEntity, printEntity, RelationshipType::PARENT);

		Relationship ifParentX = Relationship(ifEntity, assign7Entity, RelationshipType::PARENT);
		Relationship ifParentY = Relationship(ifEntity, assign8Entity, RelationshipType::PARENT);

		vector<Relationship> expectedParentRelationships{ ifParentY, ifParentX , whileParentX, whileParentPrint };

		test(procedureNode, expectedParentRelationships);
	}
}