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
	ClauseArgument ifArg = ClauseArgument::createIfArg("if");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");

	ClauseArgument stmtStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(stmtArg);
	ClauseArgument assignStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(assignArg);
	ClauseArgument callStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(callArg);
	ClauseArgument readStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(readArg);
	ClauseArgument printStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(printArg);
	ClauseArgument ifStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(ifArg);
	ClauseArgument whileStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(whileArg);

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

	vector<Entity> allEntities({ mainEntity, r1Entity, y1Entity, a2Entity, x2Entity, y2Entity, whileEntity, xCondEntity,
		zeroConstEntity, print4Entity, z4Entity, a5Entity, x5LhsEntity, x5RhsEntity, oneConstEntity, ifEntity, yCondEntity,
		fiveConstEntity, print7Entity, y7Entity, r8Entity, z8Entity, zProcedureEntity, print9Entity, z9Entity, r10Entity,
		b10Entity, call11Entity, call12Entity, yProcedureEntity, call13Entity });

	vector<Relationship> allRelationships({ usesMainY2, usesMainXCond, usesMainZ4, usesMainX5, usesMainYCond, usesMainY7,
		usesA2Y2, usesA5X5, usesW3XCond, usesW3Z4, usesW3X5, usesIf6YCond, usesIf6Y7, usesP4Z4, usesP7Y7, usesZProcedureZ9,
		usesP9Z9, 
		modifiesMainY1, modifiesMainX2, modifiesMainX5, modifiesMainZ8, modifiesA2X2, modifiesA5X5, modifiesW3X5,
		modifiesIf6Z8, modifiesR1Y1, modifiesR8Z8, modifiesZProcedureB10, modifiesR10B10, 
		callAttrC11Main, callAttrC12Y,callAttrC13Main,});
};

// =============== UNIT TESTS ====================
TEST_CASE("WithNoExactClause: test equals") {
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
		vector<ClauseArgument> lhsArgs = { stmtArg, stmtStmtNumAttributeArg };
		vector<ClauseArgument> rhsArgs = { constantArg, constantValueAttributeArg };
		first = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		second = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		testEquals(first, second, true);

		lhsArgs = { readArg, readVarNameAttributeArg };
		rhsArgs = { procedureArg, procedureProcNameAttributeArg };
		first = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		second = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		testEquals(first, second, true);
	}

	SECTION("Not equal - same type, different arguments") {
		vector<ClauseArgument> lhsArgs = { printArg, printStmtNumAttributeArg };
		vector<ClauseArgument> rhsArgs = { constantArg, constantValueAttributeArg };
		first = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		
		//different lhs
		lhsArgs = { assignArg, assignStmtNumAttributeArg };
		rhsArgs = { constantArg, constantValueAttributeArg };
		second = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		testEquals(first, second, false);

		//different rhs
		lhsArgs = { printArg, printStmtNumAttributeArg };
		rhsArgs = { callArg, callStmtNumAttributeArg };
		second = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		testEquals(first, second, false);
	}

	SECTION("Not equal - different type of with clause") {
		ClauseArgument exactArg = zLiteralArg;
		vector<ClauseArgument> lhsArgs = { callArg, callProcNameAttributeArg };
		vector<ClauseArgument> rhsArgs = { procedureArg, procedureProcNameAttributeArg };

		first = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		second = make_shared<WithOneExactClause>(exactArg, rhsArgs);
		testEquals(first, second, false);

		second = make_shared<WithBothExactClause>(exactArg, exactArg);
		testEquals(first, second, false);
		
		
	}
}

// =============== INTEGRATION TESTS ====================
TEST_CASE("WithNoExactClause: test execute - both sides equal") {
	auto testExecute = [](WithNoExactClause withNoExactClause, EntityClauseResult expected,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<ClauseResult> resPtr = withNoExactClause.execute(pkb);

			// then
			REQUIRE(dynamic_pointer_cast<EntityClauseResult>(resPtr) != nullptr);
			shared_ptr<EntityClauseResult> entityResPtr = dynamic_pointer_cast<EntityClauseResult>(resPtr);
			REQUIRE(*entityResPtr == expected);
	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	pkb->addEntities(allEntities);
	pkb->addRelationships(allRelationships);

	// ------ QPS ------
	vector<vector<ClauseArgument>> testArgs({
		{ constantArg, constantValueAttributeArg },
		{ stmtArg, stmtStmtNumAttributeArg },
		{ assignArg, assignStmtNumAttributeArg },
		{ procedureArg, procedureProcNameAttributeArg },
		{ readArg, readVarNameAttributeArg },
		{ variableArg, variableVarNameAttributeArg },
	});

	EntityClauseResult expected = EntityClauseResult::createNonEmptyNoSynonymResult();

	for (vector<ClauseArgument> eachSideArgs : testArgs) {
		WithNoExactClause clause = WithNoExactClause(eachSideArgs, eachSideArgs);
		testExecute(clause, expected, pkb);
	}
}

TEST_CASE("WithNoExactClause: test execute - both sides stmtRefSynonym.stmt#") {
	auto testExecute = [](WithNoExactClause withNoExactClause,
		shared_ptr<ClauseResult> expectedResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<ClauseResult> resPtr = withNoExactClause.execute(pkb);

			// then
			REQUIRE(resPtr->equals(expectedResult));
	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	pkb->addEntities(allEntities);
	pkb->addRelationships(allRelationships);

	// ------ QPS ------
	SECTION("Empty result (Different statement types)") {
		shared_ptr<EntityClauseResult> expected = make_shared<EntityClauseResult>(EntityClauseResult::createEmptyNoSynonymResult());
		
		//test every possible pair
		vector<vector<ClauseArgument>> notStmtStatementSynonymStmtNumPairs{
			{printArg, printStmtNumAttributeArg},
			{readArg, readStmtNumAttributeArg},
			{callArg, callStmtNumAttributeArg},
			{whileArg, whileStmtNumAttributeArg},
			{ifArg, ifStmtNumAttributeArg},
		};

		for (int i = 0; i < notStmtStatementSynonymStmtNumPairs.size(); i++) {
			for (int j = i + 1; j < notStmtStatementSynonymStmtNumPairs.size(); j++) {
				vector<ClauseArgument> lhsArgs = notStmtStatementSynonymStmtNumPairs[i];
				vector<ClauseArgument> rhsArgs = notStmtStatementSynonymStmtNumPairs[j];
				WithNoExactClause clause = WithNoExactClause(lhsArgs, rhsArgs);
				testExecute(clause, expected, pkb);
			}
		}
	}

	
	SECTION("Non-empty result (at least one side is a stmt synonym)") {
		vector<ClauseArgument> stmtArgSide{ stmtArg, stmtStmtNumAttributeArg };

		ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("ssss2");
		ClauseArgument secondStmtStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(secondStmtArg);
		vector<ClauseArgument> otherArgSide{ secondStmtArg, secondStmtStmtNumAttributeArg };
		Table expectedTable{
			{pqlR1, pqlR1}, {pqlA2, pqlA2}, {pqlW3, pqlW3}, {pqlP4, pqlP4}, {pqlA5, pqlA5},
			{pqlI6, pqlI6}, {pqlP7, pqlP7}, {pqlR8, pqlR8}, {pqlP9, pqlP9}, {pqlR10, pqlR10},
			{pqlC11, pqlC11}, {pqlC12, pqlC12}, {pqlC13, pqlC13},
		};
		vector<ClauseArgument> columnHeaders = { secondStmtArg, stmtArg };
		shared_ptr<ClauseResult> expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		WithNoExactClause clause = WithNoExactClause(stmtArgSide, otherArgSide);
		testExecute(clause, expected, pkb);
	
		otherArgSide = { printArg, printStmtNumAttributeArg };
		expectedTable = { {pqlP4, pqlP4}, {pqlP7, pqlP7}, {pqlP9, pqlP9}, }; 
		columnHeaders = { printArg, stmtArg };
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		clause = WithNoExactClause(stmtArgSide, otherArgSide);
		testExecute(clause, expected, pkb);
		
		otherArgSide = { readArg, readStmtNumAttributeArg };
		expectedTable = { {pqlR1, pqlR1}, {pqlR8, pqlR8},{pqlR10, pqlR10} };
		columnHeaders = { readArg, stmtArg };
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		clause = WithNoExactClause(otherArgSide, stmtArgSide);
		testExecute(clause, expected, pkb);
	
		otherArgSide = { callArg, callStmtNumAttributeArg };
		expectedTable = { {pqlC11, pqlC11}, {pqlC12, pqlC12}, {pqlC13, pqlC13}, };
		columnHeaders = { callArg, stmtArg };
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		clause = WithNoExactClause(otherArgSide, stmtArgSide);
		testExecute(clause, expected, pkb);

		otherArgSide = { whileArg, whileStmtNumAttributeArg };
		expectedTable = { {pqlW3, pqlW3}, };
		columnHeaders = { whileArg, stmtArg };
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		clause = WithNoExactClause(stmtArgSide, otherArgSide);
		testExecute(clause, expected, pkb);

		otherArgSide = { ifArg, ifStmtNumAttributeArg };
		expectedTable = { {pqlI6, pqlI6}, };
		columnHeaders = { ifArg, stmtArg };
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		clause = WithNoExactClause(otherArgSide, stmtArgSide);
		testExecute(clause, expected, pkb);

	}
	
	SECTION("Non-empty result (same statement type, different synonym)") {
		vector<ClauseArgument> lhsArgs{ assignArg, assignStmtNumAttributeArg };

		ClauseArgument secondAssignArg = ClauseArgument::createAssignArg("aaaa2");
		ClauseArgument secondAssignStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(secondAssignArg);
		vector<ClauseArgument> rhsArgs{ secondAssignArg, secondAssignStmtNumAttributeArg };

		Table expectedTable{ {pqlA2, pqlA2}, {pqlA5, pqlA5}, };
		vector<ClauseArgument> columnHeaders = { assignArg, secondAssignArg };
		shared_ptr<ClauseResult> expected = make_shared<ClauseResult>(columnHeaders, expectedTable);

		WithNoExactClause clause = WithNoExactClause(lhsArgs, rhsArgs);
		testExecute(clause, expected, pkb);
	}
	
}

TEST_CASE("WithNoExactClause: test execute - the rest") {
	auto testExecute = [](WithNoExactClause withNoExactClause,
		shared_ptr<ClauseResult> expectedResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<ClauseResult> resPtr = withNoExactClause.execute(pkb);


			/////=========================TODOOOOOOOOOO=======================
			// 
			// 
			// 
			// then
			REQUIRE(resPtr->equals(expectedResult));
	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	pkb->addEntities(allEntities);
	pkb->addRelationships(allRelationships);

	// ------ QPS ------
	SECTION("Same non-statement type, different synonym") {
		vector<ClauseArgument> lhsArgs{ procedureArg, procedureProcNameAttributeArg };
		ClauseArgument secondSameTypeArg = ClauseArgument::createProcedureArg("secondProcedure");
		ClauseArgument secondSameTypeArgSameAttributeArg = ClauseArgument::createProcNameAttributeArg(secondSameTypeArg);
		vector<ClauseArgument> rhsArgs{ secondSameTypeArg, secondSameTypeArgSameAttributeArg };
		vector<ClauseArgument> columnHeaders = { procedureArg, secondSameTypeArg };

		WithNoExactClause clause = WithNoExactClause(lhsArgs, rhsArgs);
		Table expectedTable{ { pqlProcMain, pqlProcMain }, { pqlProcZ, pqlProcZ }, { pqlProcY, pqlProcY } };
		shared_ptr<ClauseResult> expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);

		lhsArgs = { variableArg, variableVarNameAttributeArg };
		secondSameTypeArg = ClauseArgument::createVariableArg("secondVariable");
		secondSameTypeArgSameAttributeArg = ClauseArgument::createVarNameAttributeArg(secondSameTypeArg);
		rhsArgs = { secondSameTypeArg, secondSameTypeArgSameAttributeArg };
		columnHeaders = { variableArg, secondSameTypeArg };

		clause = WithNoExactClause(lhsArgs, rhsArgs);
		expectedTable = { { pqlVarY, pqlVarY }, { pqlVarX, pqlVarX }, { pqlVarB, pqlVarB }, { pqlVarZ, pqlVarZ } };
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);


		lhsArgs = { constantArg, constantValueAttributeArg };
		secondSameTypeArg = ClauseArgument::createConstantArg("secondConstant");
		secondSameTypeArgSameAttributeArg = ClauseArgument::createValueAttributeArg(secondSameTypeArg);
		rhsArgs = { secondSameTypeArg, secondSameTypeArgSameAttributeArg };
		columnHeaders = { constantArg, secondSameTypeArg };

		clause = WithNoExactClause(lhsArgs, rhsArgs);
		expectedTable = { { pqlConstZero, pqlConstZero }, { pqlConstOne, pqlConstOne }, { pqlConstFive, pqlConstFive } };
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);
	}

	SECTION("Different types (names)") {
		vector<ClauseArgument> lhsArgs{ procedureArg, procedureProcNameAttributeArg };
		vector<ClauseArgument> rhsArgs{ variableArg, variableVarNameAttributeArg };
		vector<ClauseArgument> columnHeaders{ procedureArg, variableArg };
		WithNoExactClause clause = WithNoExactClause(lhsArgs, rhsArgs);
		Table expectedTable{ 
			{ pqlProcY, pqlVarY }, 
			{ pqlProcZ, pqlVarZ } 
		};
		shared_ptr<ClauseResult> expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);

		lhsArgs = { procedureArg, procedureProcNameAttributeArg };
		rhsArgs = { readArg, readVarNameAttributeArg };
		columnHeaders = { procedureArg, readArg, readVarNameAttributeArg };
		clause = WithNoExactClause(lhsArgs, rhsArgs);
		expectedTable = { 
			{ pqlProcY, pqlR1, pqlVarY }, 
			{ pqlProcZ, pqlR8, pqlVarZ },
		};
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);
		
		lhsArgs = { printArg, printVarNameAttributeArg };
		rhsArgs = { readArg, readVarNameAttributeArg };
		columnHeaders = { printArg, printVarNameAttributeArg, readArg, readVarNameAttributeArg };
		clause = WithNoExactClause(lhsArgs, rhsArgs);
		expectedTable = { 
			{ pqlP7, pqlVarY, pqlR1, pqlVarY },
			{ pqlP4, pqlVarZ, pqlR8, pqlVarZ },
			{ pqlP9, pqlVarZ, pqlR8, pqlVarZ },
		};
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);

		lhsArgs = { callArg, callProcNameAttributeArg };
		rhsArgs = { printArg, printVarNameAttributeArg };
		columnHeaders = { callArg, callProcNameAttributeArg, printArg, printVarNameAttributeArg };
		clause = WithNoExactClause(lhsArgs, rhsArgs);
		expectedTable = { 
			{ pqlC12, pqlProcY, pqlP7, pqlVarY },
		};
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);
	}

	SECTION("Different types (integers)") {
		vector<ClauseArgument> lhsArgs{ constantArg, constantValueAttributeArg };
		vector<ClauseArgument> rhsArgs{ stmtArg, stmtStmtNumAttributeArg };
		vector<ClauseArgument> columnHeaders{ constantArg, stmtArg };
		WithNoExactClause clause = WithNoExactClause(lhsArgs, rhsArgs);

		Table expectedTable{
			{ pqlConstOne, pqlR1 },
			{ pqlConstFive, pqlA5 },
		};
		shared_ptr<ClauseResult> expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);

		lhsArgs = { ifArg, ifStmtNumAttributeArg };
		rhsArgs = { constantArg, constantValueAttributeArg };
		columnHeaders = { ifArg, constantArg, };
		clause = WithNoExactClause(lhsArgs, rhsArgs);
		expectedTable = {};
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);

		lhsArgs = { assignArg, assignStmtNumAttributeArg };
		rhsArgs = { constantArg, constantValueAttributeArg };
		columnHeaders = { assignArg, constantArg, };
		clause = WithNoExactClause(lhsArgs, rhsArgs);
		expectedTable = { 
			{ pqlA5, pqlConstFive },
		};
		expected = make_shared<ClauseResult>(columnHeaders, expectedTable);
		testExecute(clause, expected, pkb);
	}
}