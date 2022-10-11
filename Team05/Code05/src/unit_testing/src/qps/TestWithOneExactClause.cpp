#include "catch.hpp"
#include <qps/query/clause/WithBothExactClause.h>
#include <qps/query/clause/WithOneExactClause.h>
#include <qps/query/clause/WithNoExactClause.h>
#include <pkb/PKB.h>

using namespace std;

namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
			1. read y;
			2. x = y;
			3. while (x > 0) {
			4.	   print z;
			5.     x = x - 1;
			   }
			6. if (y < 5) then {
			7.     print y;
			   } else {
			8.     read z;
			   }
		}
		procedure z {
			9. print z;
			10.read b;
			11.call main;
			12.call y;
		}
		procedure y {
			13.call main;
		}
	*/

	// Initialise tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token bToken = Token::createNameOrKeywordToken("b");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token zeroToken = Token::createIntegerToken("0");
	Token oneToken = Token::createIntegerToken("1");
	Token fiveToken = Token::createIntegerToken("5");

	// Initialise entities
	Entity mainEntity = Entity::createProcedureEntity(mainToken);

	Entity r1Entity = Entity::createReadEntity(1);
	Entity y1Entity = Entity::createVariableEntity(1, yToken);

	Entity a2Entity = Entity::createAssignEntity(2);
	Entity x2Entity = Entity::createVariableEntity(2, xToken);
	Entity y2Entity = Entity::createVariableEntity(2, yToken);

	Entity whileEntity = Entity::createWhileEntity(3);
	Entity xCondEntity = Entity::createVariableEntity(3, xToken);
	Entity zeroConstEntity = Entity::createConstantEntity(3, zeroToken);

	Entity print4Entity = Entity::createPrintEntity(4);
	Entity z4Entity = Entity::createVariableEntity(4, zToken);

	Entity a5Entity = Entity::createAssignEntity(5);
	Entity x5LhsEntity = Entity::createVariableEntity(5, xToken);
	Entity x5RhsEntity = Entity::createVariableEntity(5, xToken);
	Entity oneConstEntity = Entity::createConstantEntity(5, oneToken);

	Entity ifEntity = Entity::createIfEntity(6);
	Entity yCondEntity = Entity::createVariableEntity(6, yToken);
	Entity fiveConstEntity = Entity::createConstantEntity(6, fiveToken);

	Entity print7Entity = Entity::createPrintEntity(7);
	Entity y7Entity = Entity::createVariableEntity(7, yToken);

	Entity r8Entity = Entity::createReadEntity(8);
	Entity z8Entity = Entity::createVariableEntity(8, zToken);

	Entity zProcedureEntity = Entity::createProcedureEntity(zToken);

	Entity print9Entity = Entity::createPrintEntity(9);
	Entity z9Entity = Entity::createVariableEntity(9, zToken);

	Entity r10Entity = Entity::createReadEntity(10);
	Entity b10Entity = Entity::createVariableEntity(10, bToken);

	Entity call11Entity = Entity::createCallEntity(11);
	Entity call12Entity = Entity::createCallEntity(12);

	Entity yProcedureEntity = Entity::createProcedureEntity(yToken);

	Entity call13Entity = Entity::createCallEntity(13);

	// Initialise uses relationships - procedure main
	Relationship usesMainY2 = Relationship::createUsesRelationship(mainEntity, y2Entity);
	Relationship usesMainXCond = Relationship::createUsesRelationship(mainEntity, xCondEntity);
	Relationship usesMainZ4 = Relationship::createUsesRelationship(mainEntity, z4Entity);
	Relationship usesMainX5 = Relationship::createUsesRelationship(mainEntity, x5RhsEntity);
	Relationship usesMainYCond = Relationship::createUsesRelationship(mainEntity, yCondEntity);
	Relationship usesMainY7 = Relationship::createUsesRelationship(mainEntity, y7Entity);

	Relationship usesA2Y2 = Relationship::createUsesRelationship(a2Entity, y2Entity);
	Relationship usesA5X5 = Relationship::createUsesRelationship(a5Entity, x5RhsEntity);

	Relationship usesW3XCond = Relationship::createUsesRelationship(whileEntity, xCondEntity);
	Relationship usesW3Z4 = Relationship::createUsesRelationship(whileEntity, z4Entity);
	Relationship usesW3X5 = Relationship::createUsesRelationship(whileEntity, x5RhsEntity);

	Relationship usesIf6YCond = Relationship::createUsesRelationship(ifEntity, yCondEntity);
	Relationship usesIf6Y7 = Relationship::createUsesRelationship(ifEntity, y7Entity);

	Relationship usesP4Z4 = Relationship::createUsesRelationship(print4Entity, z4Entity);
	Relationship usesP7Y7 = Relationship::createUsesRelationship(print7Entity, y7Entity);

	// Initialise uses relationships - procedure z
	Relationship usesZProcedureZ9 = Relationship::createUsesRelationship(zProcedureEntity, z9Entity);
	Relationship usesP9Z9 = Relationship::createUsesRelationship(print9Entity, z9Entity);

	// Initialise modifies relationships - procedure main
	Relationship modifiesMainY1 = Relationship::createModifiesRelationship(mainEntity, y1Entity);
	Relationship modifiesMainX2 = Relationship::createModifiesRelationship(mainEntity, x2Entity);
	Relationship modifiesMainX5 = Relationship::createModifiesRelationship(mainEntity, x5LhsEntity);
	Relationship modifiesMainZ8 = Relationship::createModifiesRelationship(mainEntity, z8Entity);

	Relationship modifiesA2X2 = Relationship::createModifiesRelationship(a2Entity, x2Entity);
	Relationship modifiesA5X5 = Relationship::createModifiesRelationship(a5Entity, x5LhsEntity);

	Relationship modifiesW3X5 = Relationship::createModifiesRelationship(whileEntity, x5LhsEntity);
	
	Relationship modifiesIf6Z8 = Relationship::createModifiesRelationship(ifEntity, z8Entity);
	
	Relationship modifiesR1Y1 = Relationship::createModifiesRelationship(r1Entity, y1Entity);
	Relationship modifiesR8Z8 = Relationship::createModifiesRelationship(r8Entity, z8Entity);

	// Initialise modifies relationships - procedure z
	Relationship modifiesZProcedureB10 = Relationship::createModifiesRelationship(zProcedureEntity, b10Entity);
	Relationship modifiesR10B10 = Relationship::createModifiesRelationship(r10Entity, b10Entity);

	// Initialise call statement attribute relationships
	Relationship callAttrC11Main = Relationship::createCallStmtAttributeRelationship(call11Entity, mainEntity);
	Relationship callAttrC12Y = Relationship::createCallStmtAttributeRelationship(call12Entity, yProcedureEntity);
	Relationship callAttrC13Main = Relationship::createCallStmtAttributeRelationship(call13Entity, mainEntity);

	// Initialise corresponding PQLEntities
	PQLEntity pqlR1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlP4 = PQLEntity::generateStatement(4);
	PQLEntity pqlA5 = PQLEntity::generateStatement(5);
	PQLEntity pqlI6 = PQLEntity::generateStatement(6);
	PQLEntity pqlP7 = PQLEntity::generateStatement(7);
	PQLEntity pqlR8 = PQLEntity::generateStatement(8);
	PQLEntity pqlP9 = PQLEntity::generateStatement(9);
	PQLEntity pqlR10 = PQLEntity::generateStatement(10);
	PQLEntity pqlC11 = PQLEntity::generateStatement(11);
	PQLEntity pqlC12 = PQLEntity::generateStatement(12);
	PQLEntity pqlC13 = PQLEntity::generateStatement(13);

	PQLEntity pqlProcMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlProcZ = PQLEntity::generateProcedure("z");
	PQLEntity pqlProcY = PQLEntity::generateProcedure("y");

	PQLEntity pqlVarB = PQLEntity::generateVariable("b");
	PQLEntity pqlVarX = PQLEntity::generateVariable("x");
	PQLEntity pqlVarY = PQLEntity::generateVariable("y");
	PQLEntity pqlVarZ = PQLEntity::generateVariable("z");

	PQLEntity pqlConstZero = PQLEntity::generateConstant(0);
	PQLEntity pqlConstOne = PQLEntity::generateConstant(1);
	PQLEntity pqlConstFive = PQLEntity::generateConstant(5);

	//Initialise corresponding PQLRelationships (Uses)
	PQLRelationship pqlUsesMainX = PQLRelationship(pqlProcMain, pqlVarX);
	PQLRelationship pqlUsesMainY = PQLRelationship(pqlProcMain, pqlVarY);
	PQLRelationship pqlUsesMainZ = PQLRelationship(pqlProcMain, pqlVarZ);

	PQLRelationship pqlUsesA2Y = PQLRelationship(pqlA2, pqlVarY);
	PQLRelationship pqlUsesA5X = PQLRelationship(pqlA5, pqlVarX);

	PQLRelationship pqlUsesW3X = PQLRelationship(pqlW3, pqlVarX);
	PQLRelationship pqlUsesW3Z = PQLRelationship(pqlW3, pqlVarZ);

	PQLRelationship pqlUsesIf6Y = PQLRelationship(pqlI6, pqlVarY);

	PQLRelationship pqlUsesP4Z = PQLRelationship(pqlP4, pqlVarZ);
	PQLRelationship pqlUsesP7Y = PQLRelationship(pqlP7, pqlVarY);

	PQLRelationship pqlUsesZProcedureZ = PQLRelationship(pqlProcZ, pqlVarZ);
	PQLRelationship pqlUsesP9Z = PQLRelationship(pqlP9, pqlVarZ);

	//Initialise corresponding PQLRelationships (Modifies)
	PQLRelationship pqlModifiesMainX = PQLRelationship(pqlProcMain, pqlVarX);
	PQLRelationship pqlModifiesMainY = PQLRelationship(pqlProcMain, pqlVarY);
	PQLRelationship pqlModifiesMainZ = PQLRelationship(pqlProcMain, pqlVarZ);

	PQLRelationship pqlModifiesA2X = PQLRelationship(pqlA2, pqlVarX);
	PQLRelationship pqlModifiesA5X = PQLRelationship(pqlA5, pqlVarX);

	PQLRelationship pqlModifiesW3X = PQLRelationship(pqlW3, pqlVarX);

	PQLRelationship pqlModifiesIf6Z = PQLRelationship(pqlI6, pqlVarZ);

	PQLRelationship pqlModifiesR1Y = PQLRelationship(pqlR1, pqlVarY);
	PQLRelationship pqlModifiesR8Z = PQLRelationship(pqlR8, pqlVarZ);

	PQLRelationship pqlModifiesZProcedureB = PQLRelationship(pqlProcZ, pqlVarB);
	PQLRelationship pqlModifiesR10B = PQLRelationship(pqlR10, pqlVarB);
	
	//Initiliase corresponding PQLRelationships (Call Attribute)
	PQLRelationship pqlCallAttrC11Main = PQLRelationship(pqlC11, pqlProcMain);
	PQLRelationship pqlCallAttrC12Y = PQLRelationship(pqlC12, pqlProcY);
	PQLRelationship pqlCallAttrC13Main = PQLRelationship(pqlC13, pqlProcMain);

	// Clause Arguments
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument callArg = ClauseArgument::createCallArg("call");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");

	ClauseArgument stmtStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(stmtArg);
	ClauseArgument assignStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(assignArg);
	ClauseArgument callStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(callArg);
	ClauseArgument readStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(readArg);
	ClauseArgument printStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(printArg);

	ClauseArgument callProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(callArg);
	ClauseArgument readVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(readArg);
	ClauseArgument printVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(printArg);

	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument constantArg = ClauseArgument::createConstantArg("const");
	ClauseArgument procedureArg = ClauseArgument::createProcedureArg("proc");

	ClauseArgument variableVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(variableArg);
	ClauseArgument constantValueAttributeArg = ClauseArgument::createValueAttributeArg(constantArg);
	ClauseArgument procedureProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(procedureArg);

	ClauseArgument lineZeroArg = ClauseArgument::createLineNumberArg("0");
	ClauseArgument lineOneArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineFiveArg = ClauseArgument::createLineNumberArg("5");
	ClauseArgument lineSixArg = ClauseArgument::createLineNumberArg("6");
	ClauseArgument lineTenArg = ClauseArgument::createLineNumberArg("10");

	ClauseArgument xLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument yLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument zLiteralArg = ClauseArgument::createStringLiteralArg("z");
	ClauseArgument bLiteralArg = ClauseArgument::createStringLiteralArg("b");
	ClauseArgument mainLiteralArg = ClauseArgument::createStringLiteralArg("main");

};

// =============== UNIT TESTS ====================
TEST_CASE("WithOneExactClause: test equals") {
	auto testEquals = [](shared_ptr<WithClause> first, shared_ptr<WithClause> second, bool expectedIsEqual) {
		// given

		// when
		bool actualIsEqual = first->equals(second);

		// then
		REQUIRE(actualIsEqual == expectedIsEqual);
	};

	shared_ptr<WithClause> first;
	shared_ptr<WithClause> second;
	SECTION("Equal") {
		ClauseArgument exactArg = lineOneArg;
		vector<ClauseArgument> nonExactArgs = { stmtArg, stmtStmtNumAttributeArg };
		first = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		second = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		testEquals(first, second, true);

		exactArg = zLiteralArg;
		nonExactArgs = { procedureArg, procedureProcNameAttributeArg };
		first = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		second = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		testEquals(first, second, true);
	}

	SECTION("Not equal - same type, different arguments") {
		ClauseArgument exactArg = lineOneArg;
		vector<ClauseArgument> nonExactArgs = { constantArg, constantValueAttributeArg };
		first = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		
		//different exact arg
		exactArg = lineFiveArg;
		second = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		testEquals(first, second, false);

		//different both args
		nonExactArgs = { readArg, readStmtNumAttributeArg };
		second = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		testEquals(first, second, false);

		//different non-exact args
		exactArg = lineOneArg;
		second = make_shared<WithOneExactClause>(exactArg, nonExactArgs);
		testEquals(first, second, false);
	}

	SECTION("Not equal - different type of with clause") {
		ClauseArgument exactArg = zLiteralArg;
		vector<ClauseArgument> rhsArgs = { procedureArg, procedureProcNameAttributeArg };
		first = make_shared<WithOneExactClause>(exactArg, rhsArgs);
		second = make_shared<WithBothExactClause>(exactArg, exactArg);
		testEquals(first, second, false);
		
		vector<ClauseArgument> lhsArgs = { callArg, callProcNameAttributeArg };
		second = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		testEquals(first, second, false);
	}
}

// =============== INTEGRATION TESTS ====================
TEST_CASE("WithOneExactClause: test execute - default attributes") {
	auto testExecute = [](WithOneExactClause withOneExactClause,
		EntityClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<ClauseResult> resPtr = withOneExactClause.execute(pkb);

			// then
			REQUIRE(dynamic_pointer_cast<EntityClauseResult>(resPtr) != nullptr);
			shared_ptr<EntityClauseResult> entityResPtr = dynamic_pointer_cast<EntityClauseResult>(resPtr);
			REQUIRE(*entityResPtr == expectedClauseResult);
	};


	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());

	vector<Entity> entities{ mainEntity, r1Entity, y1Entity, a2Entity, x2Entity, y2Entity, whileEntity, xCondEntity,
		zeroConstEntity, print4Entity, z4Entity, a5Entity, x5LhsEntity, x5RhsEntity, oneConstEntity, ifEntity, yCondEntity, 
		fiveConstEntity, print7Entity, y7Entity, r8Entity, z8Entity, zProcedureEntity, z9Entity, r10Entity, b10Entity, };

	vector<Relationship> relationships{ usesMainY2, usesMainXCond, usesMainZ4, usesMainX5, usesMainYCond, usesMainY7,
		usesA2Y2, usesA5X5, usesW3XCond, usesW3Z4, usesW3X5, usesIf6YCond, usesIf6Y7, usesP4Z4, usesP7Y7, usesZProcedureZ9,
		usesP9Z9, modifiesMainY1, modifiesMainX2, modifiesMainX5, modifiesMainZ8, modifiesA2X2, modifiesA5X5, modifiesW3X5,
		modifiesIf6Z8, modifiesR1Y1, modifiesR8Z8, modifiesZProcedureB10, modifiesR10B10, };

	pkb->addEntities(entities);
	pkb->addRelationships(relationships);

	// ------ QPS ------ 
	ClauseArgument exactArg = lineZeroArg;
	vector<ClauseArgument> nonExactArgs = { constantArg, constantValueAttributeArg };
	WithOneExactClause clause = WithOneExactClause(exactArg, nonExactArgs);

	vector<PQLEntity> expectedRetrievedFromPkb = { pqlConstZero };
	EntityClauseResult expectedClauseResult = EntityClauseResult(constantArg, expectedRetrievedFromPkb);

	SECTION("constant.value") {
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = lineSixArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = {};
		expectedClauseResult = EntityClauseResult(constantArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("statement.stmt#") {
		exactArg = lineTenArg;
		nonExactArgs = { stmtArg, stmtStmtNumAttributeArg };
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlR10 };
		expectedClauseResult = EntityClauseResult(stmtArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		nonExactArgs = { readArg, readStmtNumAttributeArg };
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedClauseResult = EntityClauseResult(readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		nonExactArgs = { assignArg, assignStmtNumAttributeArg };
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = {};
		expectedClauseResult = EntityClauseResult(assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("variable.varName") {
		exactArg = zLiteralArg;
		nonExactArgs = { variableArg, variableVarNameAttributeArg };
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlVarZ };
		expectedClauseResult = EntityClauseResult(variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = bLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlVarB };
		expectedClauseResult = EntityClauseResult(variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = mainLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = {};
		expectedClauseResult = EntityClauseResult(variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("procedure.procName") {
		exactArg = zLiteralArg;
		nonExactArgs = { procedureArg, procedureProcNameAttributeArg };
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlProcZ };
		expectedClauseResult = EntityClauseResult(procedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = mainLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlProcMain };
		expectedClauseResult = EntityClauseResult(procedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = bLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = {};
		expectedClauseResult = EntityClauseResult(procedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}

TEST_CASE("WithOneExactClause: test execute - non-default attributes") {
	auto testExecute = [](WithOneExactClause withOneExactClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<ClauseResult> resPtr = withOneExactClause.execute(pkb);

			// then
			REQUIRE(dynamic_pointer_cast<RelationshipClauseResult>(resPtr) != nullptr);
			shared_ptr<RelationshipClauseResult> relationshipResPtr = dynamic_pointer_cast<RelationshipClauseResult>(resPtr);
			REQUIRE(*relationshipResPtr == expectedClauseResult);
	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());

	vector<Entity> entities{ mainEntity, r1Entity, y1Entity, a2Entity, x2Entity, y2Entity, whileEntity, xCondEntity,
		zeroConstEntity, print4Entity, z4Entity, a5Entity, x5LhsEntity, x5RhsEntity, oneConstEntity, ifEntity, yCondEntity,
		fiveConstEntity, print7Entity, y7Entity, r8Entity, z8Entity, zProcedureEntity, z9Entity, r10Entity, b10Entity, 
		call11Entity, call12Entity, call13Entity};

	vector<Relationship> relationships{ usesMainY2, usesMainXCond, usesMainZ4, usesMainX5, usesMainYCond, usesMainY7,
		usesA2Y2, usesA5X5, usesW3XCond, usesW3Z4, usesW3X5, usesIf6YCond, usesIf6Y7, usesP4Z4, usesP7Y7, usesZProcedureZ9,
		usesP9Z9, 
		modifiesMainY1, modifiesMainX2, modifiesMainX5, modifiesMainZ8, modifiesA2X2, modifiesA5X5, modifiesW3X5,
		modifiesIf6Z8, modifiesR1Y1, modifiesR8Z8, modifiesZProcedureB10, modifiesR10B10, 
		callAttrC11Main, callAttrC12Y, callAttrC13Main };

	pkb->addEntities(entities);
	pkb->addRelationships(relationships);

	// ------ QPS ------ 
	ClauseArgument exactArg = zLiteralArg;
	vector<ClauseArgument> nonExactArgs = { readArg, readVarNameAttributeArg };
	WithOneExactClause clause = WithOneExactClause(exactArg, nonExactArgs);

	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlModifiesR8Z, };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(readArg, readVarNameAttributeArg, expectedRetrievedFromPkb);

	SECTION("read.varName") {
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = bLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlModifiesR10B, };
		expectedClauseResult = RelationshipClauseResult(readArg, readVarNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = xLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = {};
		expectedClauseResult = RelationshipClauseResult(readArg, readVarNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("print.varName") {
		nonExactArgs = { printArg, printVarNameAttributeArg };

		exactArg = zLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlUsesP4Z, pqlUsesP9Z};
		expectedClauseResult = RelationshipClauseResult(printArg, printVarNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = yLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlUsesP7Y, };
		expectedClauseResult = RelationshipClauseResult(printArg, printVarNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = xLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = {};
		expectedClauseResult = RelationshipClauseResult(printArg, printVarNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("call.procName") {
		nonExactArgs = { callArg, callProcNameAttributeArg };
		
		exactArg = mainLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlCallAttrC11Main, pqlCallAttrC13Main, };
		expectedClauseResult = RelationshipClauseResult(callArg, callProcNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	
		exactArg = yLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = { pqlCallAttrC12Y, };
		expectedClauseResult = RelationshipClauseResult(callArg, callProcNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		exactArg = zLiteralArg;
		clause = WithOneExactClause(exactArg, nonExactArgs);
		expectedRetrievedFromPkb = {};
		expectedClauseResult = RelationshipClauseResult(callArg, callProcNameAttributeArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

	}
}