#include "catch.hpp"
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <vector>
#include <memory>
#include <sp/dataclasses/ast/VariableASTNode.h>
#include <sp/dataclasses/ast/ConstantValueASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.cpp>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>

using namespace std;

TEST_CASE("ModifiesExtractor: test handleAssign") {


	auto handleAssign = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleAssign(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// x = x + 1
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };

	Entity assignEntity = Entity::createAssignEntity(1);

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> addNode(new ExpressionASTNode(addToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
	Entity constEntity = Entity::createConstantEntity(LINENUMBER, constToken);


	addNode->setLineNumber(1);
	assignNode->setLineNumber(1);
	x->setLineNumber(1);
	constNode->setLineNumber(1);

	assignNode->addChild(x);
	assignNode->addChild(addNode);

	addNode->addChild(x);
	addNode->addChild(constNode);


	Relationship modifiesX = Relationship{ assignEntity, LHS, RelationshipType::MODIFIES };

	vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

	handleAssign(assignNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test handleAssign exceptions") {

	auto handleAssignException = [](shared_ptr<ASTNode> ast) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// from https://stackoverflow.com/a/28797665
		bool exceptionThrown = false;
		// When
		try {
			vector<Relationship> extractedResult = extractor.handleAssign(ast);
		}
		catch(ASTException&) {
			exceptionThrown = true;
		}
		REQUIRE(exceptionThrown);
	};

	// Test exception on valid read node for handleAssign
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// read x;
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	x->setLineNumber(1);
	readNode->setLineNumber(1);

	readNode->addChild(x);

	handleAssignException(readNode);
}

TEST_CASE("ModifiesExtractor: test handleRead") {


	auto testHandleRead = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleRead(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// read x;
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };


	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	x->setLineNumber(1);
	readNode->setLineNumber(1);

	readNode->addChild(x);

	Entity lineEntity = Entity::createReadEntity(1);;
	Entity xEntity = Entity::createVariableEntity(1, xToken);

	Relationship readRelation = Relationship{ lineEntity, xEntity, RelationshipType::MODIFIES };

	vector<Relationship> expectedResult = vector<Relationship>{ readRelation };

	testHandleRead(readNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test exceptions") {
	auto handleReadException = [](shared_ptr<ASTNode> ast) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// from https://stackoverflow.com/a/28797665
		bool exceptionThrown = false;
		// When
		try {
			vector<Relationship> extractedResult = extractor.handleRead(ast);
		}
		catch (ASTException&) {
			exceptionThrown = true;
		}
		REQUIRE(exceptionThrown);
	};

	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	// should throw exception on non-read node e.g. assign node
	// x = x + 1
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token addToken = Token{ "+", TokenType::OPERATOR };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token assignToken = Token{ "=", TokenType::OPERATOR };

	Entity assignEntity = Entity::createAssignEntity(1);

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> addNode(new ExpressionASTNode(addToken));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));

	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
	Entity constEntity = Entity::createConstantEntity(LINENUMBER, constToken);


	addNode->setLineNumber(1);
	assignNode->setLineNumber(1);
	x->setLineNumber(1);
	constNode->setLineNumber(1);

	assignNode->addChild(x);
	assignNode->addChild(addNode);

	addNode->addChild(x);
	addNode->addChild(constNode);

	handleReadException(assignNode);
}

TEST_CASE("ModifiesExtractor: test handleProcedure") {


	auto handleProcedure = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleProcedure(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			x = 1;
			read y;
		}
	*/
	// Creating tokens
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token stmtLst = Token::getPlaceHolderToken();


	// Creating nodes
	shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

	shared_ptr<ASTNode> procedureNode(new ProcedureASTNode(mainToken));

	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> stmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	shared_ptr<ASTNode> y(new VariableASTNode(yToken));
	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

	x->setLineNumber(1);
	constNode->setLineNumber(1);
	y->setLineNumber(2);
	readNode->setLineNumber(2);

	procedureNode->addChild(stmtLstNode);

	stmtLstNode->addChild(assignNode);
	stmtLstNode->addChild(readNode);

	assignNode->addChild(x);
	assignNode->addChild(constNode);

	readNode->addChild(y);

	// Creating Relationship
	Entity procedureEntity = Entity::createProcedureEntity(mainToken);
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
	Entity assignEntity = Entity::createAssignEntity(assignNode->getLineNumber());
	Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

	Relationship procedureXRelation = Relationship{ procedureEntity, xEntity, RelationshipType::MODIFIES };
	Relationship procedureYRelation = Relationship{ procedureEntity, yEntity, RelationshipType::MODIFIES };
	Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
	Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ procedureXRelation, assignRelation, procedureYRelation, readRelation };

	handleProcedure(procedureNode, expectedResult);

}

TEST_CASE("ModifiesExtractor: test handleWhile") {

	auto test = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleWhile(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			1. while ( x != 0 ) {
			2. 	x = x - 1
			}
		}
	*/

	// Creating tokens
	Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token zeroToken = Token{ "1", TokenType::INTEGER };
	Token constToken = Token{ "1", TokenType::INTEGER };
	Token whileToken = Token{ WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token assignToken = Token{ EQUAL_OPERATOR, TokenType::OPERATOR };
	Token minusToken = Token{ "-", TokenType::OPERATOR};
	Token equalityToken = Token{ "!=", TokenType::OPERATOR};
	Token stmtLst = Token::getPlaceHolderToken();


	// Creating nodes
	shared_ptr<ASTNode> whileNode(new WhileASTNode(whileToken));
	whileNode->setLineNumber(1);

	shared_ptr<ASTNode> whileStmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> conditionNode(new ExpressionASTNode(equalityToken));
	conditionNode->setLineNumber(1);

	shared_ptr<ASTNode> xCond(new VariableASTNode(xToken));
	xCond->setLineNumber(1);

	
	shared_ptr<ASTNode> constZeroNode(new ConstantValueASTNode(zeroToken));
	constZeroNode->setLineNumber(1);


	shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
	assignNode->setLineNumber(2);


	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	x->setLineNumber(2);


	shared_ptr<ASTNode> exprNode(new ExpressionASTNode(minusToken));
	conditionNode->setLineNumber(2);


	shared_ptr<ASTNode> xMinus(new VariableASTNode(xToken));
	xMinus->setLineNumber(2);


	shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));
	constNode->setLineNumber(2);


	whileNode->addChild(conditionNode);
	whileNode->addChild(whileStmtLstNode);
	whileStmtLstNode->addChild(assignNode);

	conditionNode->addChild(xCond);
	conditionNode->addChild(constZeroNode);

	assignNode->addChild(x);
	assignNode->addChild(exprNode);

	exprNode->addChild(xMinus);
	exprNode->addChild(constNode);

	// Creating Relationships
	Entity whileEntity = Entity::createWhileEntity(whileNode->getLineNumber());
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity assignEntity = Entity::createAssignEntity(assignNode->getLineNumber());

	Relationship whileRelation = Relationship{ whileEntity, xEntity, RelationshipType::MODIFIES };
	Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ whileRelation, assignRelation };

	test(whileNode, expectedResult);
}

TEST_CASE("ModifiesExtractor: test handleIf") {


	auto handleProcedure = [](shared_ptr<ASTNode> ast, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.handleIf(ast);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());

		for (int i = 0; i < extractedResult.size(); i++) {
			REQUIRE(extractedResult[i].equals(expectedResult[i]));
		}

	};
	const int LINENUMBER = 1;

	Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

	/*
		procedure main {
			1. if (x == 0) then {
			2. 	   x = 1;
			}  else {
			3.     y = 2;
			}
		}
	*/
	// Creating tokens
	Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
	Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
	Token constZeroToken = Token{ "0", TokenType::INTEGER };
	Token constOneToken = Token{ "1", TokenType::INTEGER };
	Token constTwoToken = Token{ "2", TokenType::INTEGER };
	Token ifToken = Token{ IF_KEYWORD, TokenType::NAME_OR_KEYWORD };
	Token equalityToken = Token{ "==", TokenType::OPERATOR};
	Token assignToken = Token{ "=", TokenType::OPERATOR };
	Token stmtLst = Token{ "", TokenType::INVALID };

	// Creating nodes
	shared_ptr<ASTNode> ifNode(new IfASTNode(ifToken));

	shared_ptr<ASTNode> condNode(new ExpressionASTNode(equalityToken));
	shared_ptr<ASTNode> xCond(new VariableASTNode(xToken));
	shared_ptr<ASTNode> constZeroNode(new ConstantValueASTNode(constZeroToken));

	shared_ptr<ASTNode> assignXNode(new AssignASTNode(assignToken));
	shared_ptr<ASTNode> assignYNode(new AssignASTNode(assignToken));

	shared_ptr<ASTNode> thenStmtLstNode(new StatementListASTnode(stmtLst));
	shared_ptr<ASTNode> elseStmtLstNode(new StatementListASTnode(stmtLst));

	shared_ptr<ASTNode> x(new VariableASTNode(xToken));
	shared_ptr<ASTNode> constOneNode(new ConstantValueASTNode(constOneToken));
	
	shared_ptr<ASTNode> y(new VariableASTNode(yToken));
	shared_ptr<ASTNode> constTwoNode(new ConstantValueASTNode(constTwoToken));

	ifNode->addChild(condNode);
	ifNode->addChild(thenStmtLstNode);
	ifNode->addChild(elseStmtLstNode);

	condNode->addChild(xCond);
	condNode->addChild(constZeroNode);

	thenStmtLstNode->addChild(assignXNode);
	assignXNode->addChild(x);
	assignXNode->addChild(constOneNode);

	elseStmtLstNode->addChild(assignYNode);
	assignYNode->addChild(y);
	assignYNode->addChild(constTwoNode);

	// Creating Relationships
	Entity ifEntity = Entity::createIfEntity(ifNode->getLineNumber());
	Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xToken);
	Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
	Entity assignXEntity = Entity::createAssignEntity(assignXNode->getLineNumber());
	Entity assignYEntity = Entity::createAssignEntity(assignYNode->getLineNumber());

	Relationship xRelation = Relationship{ ifEntity, xEntity, RelationshipType::MODIFIES };
	Relationship yRelation = Relationship{ ifEntity, yEntity, RelationshipType::MODIFIES };
	Relationship assignXRelation = Relationship{ assignXEntity, xEntity, RelationshipType::MODIFIES };
	Relationship assignYRelation = Relationship{ assignYEntity, yEntity, RelationshipType::MODIFIES };


	vector<Relationship> expectedResult = vector<Relationship>{ xRelation, assignXRelation, yRelation, assignYRelation };

	handleProcedure(ifNode, expectedResult);

}


TEST_CASE("ModifiesExtractor: test extract") {
	auto testExtract = [](shared_ptr<ASTNode> rootNode, vector<Relationship> expectedResult) {
		// Given
		ModifiesExtractor extractor = ModifiesExtractor();

		// When
		vector<Relationship> extractedResult = extractor.extract(rootNode);

		// Then
		REQUIRE(expectedResult.size() == extractedResult.size());
		
		/* 
			Refactored this test case: cannot guarantee that order of traversal in extractedResult same as order in expectedResult. 
			Generalised this test case to check if extracted relationships are contained in expected result
		*/
		for (int i = 0; i < extractedResult.size(); i++) {
			bool isInExpectedResult = false;
			for (int j = 0; j < expectedResult.size(); j++) {
				isInExpectedResult = isInExpectedResult || extractedResult[i].equals(expectedResult[j]);
				if (isInExpectedResult) break;
			}
			REQUIRE(isInExpectedResult);
		}
		
	};
	
	SECTION("Test extract on single statement") {
		const int LINENUMBER = 1;

		Token leftToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Entity LHS = Entity::createVariableEntity(LINENUMBER, leftToken);

		// x = x + 1 + y
		Token xToken = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token addToken = Token{ "+", TokenType::OPERATOR };
		Token constToken = Token{ "1", TokenType::INTEGER };
		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };
		Entity assignEntity = Entity::createAssignEntity(LINENUMBER);;


		Entity yEntity = Entity::createVariableEntity(LINENUMBER, yToken);

		shared_ptr<ASTNode> LHSNode(new VariableASTNode(xToken));

		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));

		shared_ptr<ASTNode> addNode1(new ExpressionASTNode(addToken));
		shared_ptr<ASTNode> addNode2(new ExpressionASTNode(addToken));

		shared_ptr<ASTNode> x(new VariableASTNode(xToken));
		shared_ptr<ASTNode> y(new VariableASTNode(yToken));

		shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

		Entity constEntity = Entity::createConstantEntity(LINENUMBER, constToken);


		addNode1->setLineNumber(1);
		assignNode->setLineNumber(1);
		x->setLineNumber(1);
		y->setLineNumber(1);
		constNode->setLineNumber(1);

		assignNode->addChild(x);
		assignNode->addChild(addNode1);

		addNode1->addChild(x);
		addNode1->addChild(addNode2);
		addNode2->addChild(constNode);
		addNode2->addChild(y);


		Relationship modifiesX = Relationship{ assignEntity, LHS, RelationshipType::MODIFIES };

		vector<Relationship> expectedResult = vector<Relationship>{ modifiesX };

		testExtract(assignNode, expectedResult);
	}
	
	
	SECTION("Test extract from root program node") {
		/*
			procedure main {
				1. x = 1
			}

			procedure readY {
				2. read y
			}
		*/
		
		const int LINENUMBER = 1;

		// Create tokens
		// main
		Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
		Token xTokenMain = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token constToken = Token{ "1", TokenType::INTEGER };

		// readY
		Token readYToken = Token{ "readY", TokenType::NAME_OR_KEYWORD };
		Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };

		// tokens for full program
		Token stmtLstToken = Token::getPlaceHolderToken();
		Token rootNodeToken = Token::getPlaceHolderToken();

		// creating nodes
		shared_ptr<ASTNode> rootProgramNode(new ProgramASTNode(rootNodeToken));
		shared_ptr<ASTNode> procedureMainNode(new ProcedureASTNode(mainToken));
		shared_ptr<ASTNode> procedureReadYNode(new ProcedureASTNode(readYToken));

		shared_ptr<ASTNode> stmtLstMain(new StatementListASTnode(stmtLstToken));
		shared_ptr<ASTNode> stmtLstReadY(new StatementListASTnode(stmtLstToken));

		shared_ptr<ASTNode> assignNode(new AssignASTNode(assignToken));
		shared_ptr<ASTNode> readNode(new ReadASTNode(readToken));

		shared_ptr<ASTNode> x(new VariableASTNode(xTokenMain));
		shared_ptr<ASTNode> y(new VariableASTNode(yToken));
		shared_ptr<ASTNode> constNode(new ConstantValueASTNode(constToken));

		x->setLineNumber(1);
		constNode->setLineNumber(1);
		y->setLineNumber(2);
		readNode->setLineNumber(2);

		rootProgramNode->addChild(procedureMainNode);
		rootProgramNode->addChild(procedureReadYNode);

		procedureMainNode->addChild(stmtLstMain);
		procedureReadYNode->addChild(stmtLstReadY);

		stmtLstMain->addChild(assignNode);
		stmtLstReadY->addChild(readNode);

		assignNode->addChild(x);
		assignNode->addChild(constNode);

		readNode->addChild(y);

		// Create entities
		Entity programEntity = Entity::createProgramEntity();
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
		Entity procedureReadYEntity = Entity::createProcedureEntity(readYToken);


		Entity xEntity = Entity::createVariableEntity(x->getLineNumber(), xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(assignNode->getLineNumber());

		Entity yEntity = Entity::createVariableEntity(y->getLineNumber(), yToken);
		Entity readEntity = Entity::createReadEntity(readNode->getLineNumber());

		Relationship procedureXRelationship = Relationship{ procedureMainEntity, xEntity, RelationshipType::MODIFIES };
		Relationship procedureYRelationship = Relationship{ procedureReadYEntity, yEntity, RelationshipType::MODIFIES };
		Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
		Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };

		vector <Relationship> expectedResult = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };

		testExtract(rootProgramNode, expectedResult);
		
	}
	

	


}