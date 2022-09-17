#include "catch.hpp"

#include <sp/design_extractor/FollowsTExtractor.h>
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

TEST_CASE("FollowsTExtractor: test extract") {
	auto test = [](shared_ptr<ASTNode> astNode, vector<Relationship> expectedResult) {
		// Given
		FollowsTExtractor extractor = FollowsTExtractor();

		// When
		vector<Relationship> extractedRelationships = extractor.extract(astNode);

		// Then
		REQUIRE(extractedRelationships.size() == expectedResult.size());

		for (int i = 0; i < extractedRelationships.size(); i++)
		{
			REQUIRE(extractedRelationships[i].equals(expectedResult[i]));
		}
	};

	SECTION("Test nested - empty results") {
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

		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token readToken = Token::createReadToken();
		Token whileToken = Token::createWhileToken();
		Token ifToken = Token::createIfToken();
		Token greaterThan = Token::createGreaterThanToken();

		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));
		shared_ptr<ASTNode> mainStmtList(new StatementListASTNode(Token::getPlaceHolderToken()));
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

		shared_ptr<ASTNode> whileStmtLstNode(new StatementListASTNode(Token::getPlaceHolderToken()));
		shared_ptr<ASTNode> whileCond(new ExpressionASTNode(greaterThan));

		whileNode->addChild(whileCond);
		whileNode->addChild(whileStmtLstNode);

		shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));
		shared_ptr<ASTNode> ifCond(new ExpressionASTNode(greaterThan));
		ifNode->setLineNumber(2);

		whileStmtLstNode->addChild(ifNode);

		shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTNode(Token::getPlaceHolderToken()));
		shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTNode(Token::getPlaceHolderToken()));

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

		vector<Relationship> expectedFollowsTRelationships{};

		test(procedureNode, expectedFollowsTRelationships);
	}

	SECTION("Test nested - non empty results") {
		/*
			procedure main {
			1.	if (x < 0) then {
			2.		y = y + 1;
			3.		while (x < 0) {
			4.			x = x + 2;
			5.			read y;
					}
			6.		if () then {} else {}
			7.		print y;
				} else {}
			}
		*/

		// Variables are omitted for brevity. FollowsTExtractor does not interact with variables. 
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token readToken = Token::createReadToken();
		Token printToken = Token::createReadToken();
		Token ifToken = Token::createIfToken();
		Token whileToken = Token::createWhileToken();
		Token lessThanToken = Token::createLessThanToken();
		Token equalsToken = Token::createEqualsToken();
		Token placeholderToken = Token::createPlaceholderToken();

		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));
		shared_ptr<ASTNode> mainStmtListNode(new StatementListASTNode(placeholderToken));
		procedureNode->addChild(mainStmtListNode);

		shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));
		shared_ptr<ASTNode> ifCond(new ExpressionASTNode(lessThanToken));
		ifNode->setLineNumber(1);

		mainStmtListNode->addChild(ifNode);

		shared_ptr<ASTNode> thenStmtListNode(new StatementListASTNode(placeholderToken));
		shared_ptr<ASTNode> elseStmtListNode(new StatementListASTNode(placeholderToken));

		ifNode->addChild(ifCond);
		ifNode->addChild(thenStmtListNode);
		ifNode->addChild(elseStmtListNode);

		shared_ptr<ASTNode> assign2Node(new AssignASTNode(equalsToken));
		assign2Node->setLineNumber(2);

		thenStmtListNode->addChild(assign2Node);

		shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
		whileNode->setLineNumber(3);

		thenStmtListNode->addChild(whileNode);

		shared_ptr<ASTNode> whileStmtListNode(new StatementListASTNode(placeholderToken));
		shared_ptr<ASTNode> whileCond(new ExpressionASTNode(lessThanToken));

		whileNode->addChild(whileCond);
		whileNode->addChild(whileStmtListNode);

		shared_ptr<ASTNode> assign4Node(new AssignASTNode(equalsToken));
		assign4Node->setLineNumber(4);

		whileStmtListNode->addChild(assign4Node);

		shared_ptr<ASTNode> readYNode(new ReadASTNode(readToken));
		readYNode->setLineNumber(5);

		whileStmtListNode->addChild(readYNode);

		shared_ptr<ASTNode> nestedIfNode(new IfASTNode(ifToken));
		nestedIfNode->setLineNumber(6);

		thenStmtListNode->addChild(nestedIfNode);

		shared_ptr<ASTNode> emptyIfCond(new ExpressionASTNode(lessThanToken));
		shared_ptr<ASTNode> emptyThenStmtListNode(new StatementListASTNode(placeholderToken));
		shared_ptr<ASTNode> emptyElseStmtListNode(new StatementListASTNode(placeholderToken));

		nestedIfNode->addChild(emptyIfCond);
		nestedIfNode->addChild(emptyThenStmtListNode);
		nestedIfNode->addChild(emptyElseStmtListNode);

		shared_ptr<ASTNode> printNode(new PrintASTNode(printToken));
		printNode->setLineNumber(7);

		thenStmtListNode->addChild(printNode);

		// Create entities
		Entity ifEntity = Entity::createIfEntity(1);
		Entity assign2Entity = Entity::createAssignEntity(2);
		Entity whileEntity = Entity::createWhileEntity(3);
		Entity assign4Entity = Entity::createAssignEntity(4);
		Entity readYEntity = Entity::createReadEntity(5);
		Entity nestedIfEntity = Entity::createIfEntity(6);
		Entity printEntity = Entity::createPrintEntity(7);

		// Create relationships
		Relationship followsTA2While = Relationship::createFollowsTRelationship(assign2Entity, whileEntity);
		Relationship followsTA2NestedIf = Relationship::createFollowsTRelationship(assign2Entity, nestedIfEntity);
		Relationship followsTA2Print = Relationship::createFollowsTRelationship(assign2Entity, printEntity);
		
		Relationship followsTWhileNestedIf = Relationship::createFollowsTRelationship(whileEntity, nestedIfEntity);
		Relationship followsTWhilePrint = Relationship::createFollowsTRelationship(whileEntity, printEntity);

		Relationship followsTA4ReadY = Relationship::createFollowsTRelationship(assign4Entity, readYEntity);

		Relationship followsTNestedIfPrint = Relationship::createFollowsTRelationship(nestedIfEntity, printEntity);

		vector<Relationship> expectedFollowsTRelationships{ followsTA2While, followsTA2NestedIf, followsTA2Print, followsTWhileNestedIf, 
			followsTWhilePrint, followsTA4ReadY, followsTNestedIfPrint };

		test(procedureNode, expectedFollowsTRelationships);
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
		shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

		// Line 1 (x = 3)
		shared_ptr<ASTNode> mainStmtList(new StatementListASTNode(stmtListToken));
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
		shared_ptr<ASTNode> whileStmtList(new StatementListASTNode(stmtListToken));
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
		shared_ptr<ASTNode> thenStmtList(new StatementListASTNode(stmtListToken));
		shared_ptr<ASTNode> assign7Node(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> x7Node(new VariableASTNode(xToken));
		shared_ptr<ASTNode> y7Node(new VariableASTNode(yToken));

		assign7Node->setLineNumber(7);
		x7Node->setLineNumber(7);
		y7Node->setLineNumber(7);

		// Line 8 (y = x)
		shared_ptr<ASTNode> elseStmtList(new StatementListASTNode(stmtListToken));
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
		Entity assign1Entity = Entity::createAssignEntity(assign1Node->getLineNumber());
		Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

		Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());

		Entity assign4Entity = Entity::createAssignEntity(assign4Node->getLineNumber());
		Entity printEntity = Entity::createPrintEntity(printNode->getLineNumber());

		Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());

		// Creating Relationships
		Relationship followsTAssign1Read = Relationship::createFollowsTRelationship(assign1Entity, readEntity);
		Relationship followsTAssign1While = Relationship::createFollowsTRelationship(assign1Entity, whileEntity);
		Relationship followsTAssign1If = Relationship::createFollowsTRelationship(assign1Entity, ifEntity);

		Relationship followsTReadWhile = Relationship::createFollowsTRelationship(readEntity, whileEntity);
		Relationship followsTReadIf = Relationship::createFollowsTRelationship(readEntity, ifEntity);

		Relationship followsTWhileIf = Relationship::createFollowsTRelationship(whileEntity, ifEntity);

		Relationship followsTAssign4Print = Relationship::createFollowsTRelationship(assign4Entity, printEntity);

		vector<Relationship> expectedParentRelationships{ followsTAssign1Read, followsTAssign1While, followsTAssign1If, followsTReadWhile,
			followsTReadIf, followsTWhileIf, followsTAssign4Print};

		test(procedureNode, expectedParentRelationships);
	}
}
