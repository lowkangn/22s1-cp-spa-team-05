#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/UsesPClause.h>
#include <qps/query/clause/UsesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("UsesPClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		UsesPClause usesPClause = UsesPClause(lhs, rhs);

		// when
		bool actual = usesPClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument secondProcArg = ClauseArgument::createProcedureArg("sp1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> usesPClauseAgain(new UsesPClause(firstProcArg, firstVarArg));
		testEquals(usesPClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		shared_ptr<RelationshipClause> usesPClauseDiffLhsString(new UsesPClause(secondProcArg, firstVarArg));
		testEquals(usesPClauseDiffLhsString, false);

		shared_ptr<RelationshipClause> usesPClauseDiffRhsString(new UsesPClause(firstProcArg, secondVarArg));
		testEquals(usesPClauseDiffRhsString, false);

		shared_ptr<RelationshipClause> usesPClauseDiffStrings(new UsesPClause(secondProcArg, secondVarArg));
		testEquals(usesPClauseDiffStrings, false);
	}

	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument callArg = ClauseArgument::createCallArg("c");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	list<ClauseArgument> otherArguments = list<ClauseArgument>{ stmtArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg };

	SECTION("Not equal, different types") {
		for (ClauseArgument argument : otherArguments) {
			shared_ptr<RelationshipClause> usesPClauseDiffLhsArg(new UsesPClause(firstProcArg, argument));
			testEquals(usesPClauseDiffLhsArg, false);

			shared_ptr<RelationshipClause> usesPClauseDiffRhsArg(new UsesPClause(argument, firstVarArg));
			testEquals(usesPClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a UsesPClause") {
		shared_ptr<RelationshipClause> usesSClause(new UsesPClause(stmtArg, firstVarArg));
		testEquals(usesSClause, false);

		// TODO: Add more when other Clause classes are implemented
		shared_ptr<RelationshipClause> modifiesPClause(new ModifiesPClause(firstProcArg, firstVarArg));
		testEquals(modifiesPClause, false);

		shared_ptr<RelationshipClause> modifiesSClause(new ModifiesSClause(assignArg, firstVarArg));
		testEquals(modifiesSClause, false);
	}
}

// =============== INTEGRATION TESTS ====================
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
			1. read y;
			2. x = y;
			3. while (x > 0) {
			4.	   call noUses;
			5.     x = x - 1;
			   }
			6. if (y < 5) then {
			7.     y = 3;
			   } else {
			8.     call usesXAndY;
			   }
		}

		procedure test {
			9.	z = 2;
			10. call usesXAndY;
		}

		procedure noUses {
			11. read z;
		}

		procedure usesXAndY {
			12. print y;
			13. call usesX;
		}

		procedure usesX {
			14. y = x;
		}
	*/

	// Initialise tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token testToken = Token::createNameOrKeywordToken("test");
	Token noUsesToken = Token::createNameOrKeywordToken("noUses");
	Token usesXYToken = Token::createNameOrKeywordToken("usesXAndY");
	Token usesXToken = Token::createNameOrKeywordToken("usesX");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token zeroToken = Token::createIntegerToken("0");
	Token oneToken = Token::createIntegerToken("1");
	Token twoToken = Token::createIntegerToken("2");
	Token threeToken = Token::createIntegerToken("3");
	Token fiveToken = Token::createIntegerToken("5");

	// Initialise entities
	Entity mainEntity = Entity::createProcedureEntity(mainToken);
	Entity testEntity = Entity::createProcedureEntity(testToken);
	Entity noUsesEntity = Entity::createProcedureEntity(noUsesToken);
	Entity usesXYEntity = Entity::createProcedureEntity(usesXYToken);
	Entity usesXEntity = Entity::createProcedureEntity(usesXToken);

	Entity read1Entity = Entity::createReadEntity(1);
	Entity y1Entity = Entity::createVariableEntity(1, yToken);

	Entity a2Entity = Entity::createAssignEntity(2);
	Entity x2Entity = Entity::createVariableEntity(2, xToken);
	Entity y2Entity = Entity::createVariableEntity(2, yToken);

	Entity whileEntity = Entity::createWhileEntity(3);
	Entity xCondEntity = Entity::createVariableEntity(3, xToken);
	Entity zeroConstEntity = Entity::createConstantEntity(3, zeroToken);

	Entity call4Entity = Entity::createCallEntity(4);

	Entity a5Entity = Entity::createAssignEntity(5);
	Entity x5LhsEntity = Entity::createVariableEntity(5, xToken);
	Entity x5RhsEntity = Entity::createVariableEntity(5, xToken);
	Entity oneConstEntity = Entity::createConstantEntity(5, oneToken);

	Entity ifEntity = Entity::createIfEntity(6);
	Entity yCondEntity = Entity::createVariableEntity(6, yToken);
	Entity fiveConstEntity = Entity::createConstantEntity(6, fiveToken);

	Entity a7Entity = Entity::createAssignEntity(7);
	Entity y7Entity = Entity::createVariableEntity(7, yToken);
	Entity threeConstEntity = Entity::createConstantEntity(7, threeToken);

	Entity call8Entity = Entity::createCallEntity(8);

	Entity a9Entity = Entity::createAssignEntity(9);
	Entity z9Entity = Entity::createVariableEntity(9, zToken);
	Entity twoConstEntity = Entity::createConstantEntity(9, twoToken);

	Entity call10Entity = Entity::createCallEntity(10);

	Entity read11Entity = Entity::createReadEntity(11);
	Entity z11Entity = Entity::createVariableEntity(11, zToken);

	Entity print12Entity = Entity::createPrintEntity(12);
	Entity y12Entity = Entity::createVariableEntity(12, yToken);

	Entity call13Entity = Entity::createCallEntity(13);

	Entity a14Entity = Entity::createAssignEntity(14);
	Entity y14Entity = Entity::createVariableEntity(14, yToken);
	Entity x14Entity = Entity::createVariableEntity(14, xToken);

	// Initialise uses relationships
	Relationship usesMainY2 = Relationship::createUsesRelationship(mainEntity, y2Entity);
	Relationship usesMainXCond = Relationship::createUsesRelationship(mainEntity, xCondEntity);
	Relationship usesMainX5 = Relationship::createUsesRelationship(mainEntity, x5RhsEntity);
	Relationship usesMainYCond = Relationship::createUsesRelationship(mainEntity, yCondEntity);
	Relationship usesMainY12 = Relationship::createUsesRelationship(mainEntity, y12Entity);
	Relationship usesMainX14 = Relationship::createUsesRelationship(mainEntity, x14Entity);

	Relationship usesA2Y2 = Relationship::createUsesRelationship(a2Entity, y2Entity);

	Relationship usesW3XCond = Relationship::createUsesRelationship(whileEntity, xCondEntity);
	Relationship usesW3X5 = Relationship::createUsesRelationship(whileEntity, x5RhsEntity);

	Relationship usesIf6YCond = Relationship::createUsesRelationship(ifEntity, yCondEntity);
	Relationship usesIf6Y12 = Relationship::createUsesRelationship(ifEntity, y12Entity);
	Relationship usesIf6X14 = Relationship::createUsesRelationship(ifEntity, x14Entity);

	Relationship usesC8Y12 = Relationship::createUsesRelationship(call8Entity, y12Entity);
	Relationship usesC8X14 = Relationship::createUsesRelationship(call8Entity, x14Entity);

	Relationship usesTestY12 = Relationship::createUsesRelationship(testEntity, y12Entity);
	Relationship usesTestX14 = Relationship::createUsesRelationship(testEntity, x14Entity);

	Relationship usesC10Y12 = Relationship::createUsesRelationship(call10Entity, y12Entity);
	Relationship usesC10X14 = Relationship::createUsesRelationship(call10Entity, x14Entity);

	Relationship usesUsesXYY12 = Relationship::createUsesRelationship(usesXYEntity, y12Entity);
	Relationship usesUsesXYX14 = Relationship::createUsesRelationship(usesXYEntity, x14Entity);

	Relationship usesP12Y12 = Relationship::createUsesRelationship(print12Entity, y12Entity);
	Relationship usesC13X14 = Relationship::createUsesRelationship(call13Entity, x14Entity);

	Relationship usesUsesXX14 = Relationship::createUsesRelationship(usesXEntity, x14Entity);

	Relationship usesA14X14 = Relationship::createUsesRelationship(a14Entity, x14Entity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlTest = PQLEntity::generateProcedure("test");
	PQLEntity pqlUsesXY = PQLEntity::generateProcedure("usesXAndY");
	PQLEntity pqlUsesX = PQLEntity::generateProcedure("usesX");

	PQLEntity pqlVarX = PQLEntity::generateVariable("x");
	PQLEntity pqlVarY = PQLEntity::generateVariable("y");

	PQLRelationship pqlUsesMainY = PQLRelationship(pqlMain, pqlVarY);
	PQLRelationship pqlUsesMainX = PQLRelationship(pqlMain, pqlVarX);

	PQLRelationship pqlUsesTestY = PQLRelationship(pqlTest, pqlVarY);
	PQLRelationship pqlUsesTestX = PQLRelationship(pqlTest, pqlVarX);

	PQLRelationship pqlUsesUsesXYY = PQLRelationship(pqlUsesXY, pqlVarY);
	PQLRelationship pqlUsesUsesXYX = PQLRelationship(pqlUsesXY, pqlVarX);

	PQLRelationship pqlUsesUsesXX = PQLRelationship(pqlUsesX, pqlVarX);

	// Clause Arguments
	ClauseArgument procedureArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument mainLiteralArg = ClauseArgument::createStringLiteralArg("main");
	ClauseArgument testLiteralArg = ClauseArgument::createStringLiteralArg("test");
	ClauseArgument usesXYLiteralArg = ClauseArgument::createStringLiteralArg("usesXAndY");
	ClauseArgument noUsesLiteralArg = ClauseArgument::createStringLiteralArg("noUses");
	ClauseArgument xLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument yLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument zLiteralArg = ClauseArgument::createStringLiteralArg("z");
};

TEST_CASE("UsesPClause: test execute") {
	auto testExecute = [](UsesPClause usesSClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = usesSClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;
			
			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());

	vector<Entity> entities{ mainEntity, testEntity, noUsesEntity, usesXYEntity, usesXEntity, read1Entity, y1Entity,
			a2Entity, x2Entity, y2Entity, whileEntity, xCondEntity, zeroConstEntity, call4Entity, a5Entity, x5LhsEntity,
			x5RhsEntity, oneConstEntity, ifEntity, yCondEntity, fiveConstEntity, a7Entity, y7Entity, threeConstEntity,
			call8Entity, a9Entity, z9Entity, twoConstEntity, call10Entity, read11Entity, z11Entity, print12Entity,
			y12Entity, call13Entity, a14Entity, y14Entity, x14Entity };

	vector<Relationship> relationships{ usesMainY2, usesMainXCond, usesMainX5, usesMainYCond, usesMainY12, usesMainX14,
			usesA2Y2, usesW3XCond, usesW3X5, usesIf6YCond, usesIf6Y12, usesIf6X14, usesC8Y12, usesC8X14, usesTestY12,
			usesTestX14, usesC10Y12, usesC10X14, usesUsesXYY12, usesUsesXYX14, usesP12Y12, usesC13X14, usesUsesXX14,
			usesA14X14 };

	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	UsesPClause clause = UsesPClause(procedureArg, variableArg);

	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlUsesMainY, pqlUsesMainX, pqlUsesTestY, pqlUsesTestX, pqlUsesUsesXYY,
			pqlUsesUsesXYX, pqlUsesUsesXX };

	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(procedureArg, variableArg, expectedRetrievedFromPkb);

	SECTION("One procedure synonym and one variable synonym - all statement uses") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure string literal - non empty results") {
		clause = UsesPClause(mainLiteralArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesMainY, pqlUsesMainX};
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesPClause(testLiteralArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesTestY, pqlUsesTestX };
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure string literal - empty results") {
		expectedRetrievedFromPkb = {};

		clause = UsesPClause(noUsesLiteralArg, variableArg);
		expectedClauseResult = RelationshipClauseResult(noUsesLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Variable string literal - non empty results") {
		clause = UsesPClause(procedureArg, yLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesMainY, pqlUsesTestY, pqlUsesUsesXYY };
		expectedClauseResult = RelationshipClauseResult(procedureArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesPClause(procedureArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesMainX, pqlUsesTestX, pqlUsesUsesXYX, pqlUsesUsesXX };
		expectedClauseResult = RelationshipClauseResult(procedureArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Variable string literal - non empty results") {
		expectedRetrievedFromPkb = {};

		clause = UsesPClause(procedureArg, zLiteralArg);
		expectedClauseResult = RelationshipClauseResult(procedureArg, zLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure and variable string literals - non empty results") {
		clause = UsesPClause(mainLiteralArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesMainX };
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesPClause(testLiteralArg, yLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesTestY };
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure and variable string literals - empty results") {
		expectedRetrievedFromPkb = {};

		clause = UsesPClause(usesXYLiteralArg, zLiteralArg);
		expectedClauseResult = RelationshipClauseResult(usesXYLiteralArg, zLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}
