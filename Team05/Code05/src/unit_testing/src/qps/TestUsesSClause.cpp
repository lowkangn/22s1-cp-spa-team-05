#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("UsesSClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		UsesSClause usesSClause = UsesSClause(lhs, rhs);

		// when
		bool actual = usesSClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> usesSClauseAgain(new UsesSClause(firstStmtArg, firstVarArg));
		testEquals(usesSClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		shared_ptr<RelationshipClause> usesSClauseDiffLhsString(new UsesSClause(secondStmtArg, firstVarArg));
		testEquals(usesSClauseDiffLhsString, false);

		shared_ptr<RelationshipClause> usesSClauseDiffRhsString(new UsesSClause(firstStmtArg, secondVarArg));
		testEquals(usesSClauseDiffRhsString, false);

		shared_ptr<RelationshipClause> usesSClauseDiffStrings(new UsesSClause(secondStmtArg, secondVarArg));
		testEquals(usesSClauseDiffStrings, false);
	}

	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument callArg = ClauseArgument::createCallArg("c");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument stringLitArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	list<ClauseArgument> otherArguments = list<ClauseArgument>{ procArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg };

	SECTION("Not equal, different types") {
		for (ClauseArgument argument : otherArguments) {
			shared_ptr<RelationshipClause> usesSClauseDiffLhsArg(new UsesSClause(firstStmtArg, argument));
			testEquals(usesSClauseDiffLhsArg, false);

			shared_ptr<RelationshipClause> usesSClauseDiffRhsArg(new UsesSClause(argument, firstVarArg));
			testEquals(usesSClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a UsesSClause") {
		shared_ptr<RelationshipClause> usesPClause(new UsesPClause(procArg, firstVarArg));
		testEquals(usesPClause, false);

		// TODO: Add more when other Clause classes are implemented
		shared_ptr<RelationshipClause> modifiesPClause(new ModifiesPClause(procArg, firstVarArg));
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
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlA5 = PQLEntity::generateStatement(5);
	PQLEntity pqlI6 = PQLEntity::generateStatement(6);
	PQLEntity pqlC8 = PQLEntity::generateStatement(8);
	PQLEntity pqlC10 = PQLEntity::generateStatement(10);
	PQLEntity pqlP12 = PQLEntity::generateStatement(12);
	PQLEntity pqlC13 = PQLEntity::generateStatement(13);
	PQLEntity pqlA14 = PQLEntity::generateStatement(14);
	
	PQLEntity pqlVarX = PQLEntity::generateVariable("x");
	PQLEntity pqlVarY = PQLEntity::generateVariable("y");

	PQLRelationship pqlUsesA2Y = PQLRelationship(pqlA2, pqlVarY);

	PQLRelationship pqlUsesW3X = PQLRelationship(pqlW3, pqlVarX);

	PQLRelationship pqlUsesIf6Y = PQLRelationship(pqlI6, pqlVarY);
	PQLRelationship pqlUsesIf6X = PQLRelationship(pqlI6, pqlVarX);
	PQLRelationship pqlUsesC8Y = PQLRelationship(pqlC8, pqlVarY);
	PQLRelationship pqlUsesC8X = PQLRelationship(pqlC8, pqlVarX);

	PQLRelationship pqlUsesC10Y = PQLRelationship(pqlC10, pqlVarY);
	PQLRelationship pqlUsesC10X = PQLRelationship(pqlC10, pqlVarX);

	PQLRelationship pqlUsesP12Y = PQLRelationship(pqlP12, pqlVarY);
	PQLRelationship pqlUsesC13X = PQLRelationship(pqlC13, pqlVarX);

	PQLRelationship pqlUsesA14X = PQLRelationship(pqlA14, pqlVarX);

	// Clause Arguments
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument ifArg = ClauseArgument::createIfArg("if");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("while");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument callArg = ClauseArgument::createCallArg("c");
	ClauseArgument lineOneArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineTwoArg = ClauseArgument::createLineNumberArg("2");
	ClauseArgument lineSixArg = ClauseArgument::createLineNumberArg("6");
	ClauseArgument lineSevenArg = ClauseArgument::createLineNumberArg("7");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");
	ClauseArgument xLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument yLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument zLiteralArg = ClauseArgument::createStringLiteralArg("z");
};

TEST_CASE("UsesSClause: test execute") {
	auto testExecute = [](UsesSClause usesSClause,
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
	UsesSClause clause = UsesSClause(stmtArg, variableArg);

	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlUsesA2Y, pqlUsesW3X, pqlUsesIf6Y, pqlUsesIf6X, pqlUsesC8Y,
			pqlUsesC8X, pqlUsesC10Y, pqlUsesC10X, pqlUsesP12Y, pqlUsesC13X, pqlUsesA14X };

	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(stmtArg, variableArg,
		expectedRetrievedFromPkb);

	SECTION("One stmt synonym and one variable synonym - all statement uses") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - non empty results") {
		clause = UsesSClause(ifArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesIf6Y, pqlUsesIf6X };
		expectedClauseResult = RelationshipClauseResult(ifArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(whileArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesW3X };
		expectedClauseResult = RelationshipClauseResult(whileArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(assignArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesA2Y, pqlUsesA14X };
		expectedClauseResult = RelationshipClauseResult(assignArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(printArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesP12Y };
		expectedClauseResult = RelationshipClauseResult(printArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(callArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesC8Y, pqlUsesC8X, pqlUsesC10Y, pqlUsesC10X, pqlUsesC13X };
		expectedClauseResult = RelationshipClauseResult(callArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - empty results") {
;		expectedRetrievedFromPkb = {};

		clause = UsesSClause(readArg, variableArg);
		expectedClauseResult = RelationshipClauseResult(readArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line numbers - non empty results") {
		clause = UsesSClause(lineTwoArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesA2Y };
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(lineSixArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesIf6Y, pqlUsesIf6X };
		expectedClauseResult = RelationshipClauseResult(lineSixArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line numbers - empty results") {
		expectedRetrievedFromPkb = {};

		clause = UsesSClause(lineOneArg, variableArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(lineSevenArg, variableArg);
		expectedClauseResult = RelationshipClauseResult(lineSevenArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("String literal - non empty results") {
		clause = UsesSClause(whileArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesW3X };
		expectedClauseResult = RelationshipClauseResult(whileArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(assignArg, yLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesA2Y };
		expectedClauseResult = RelationshipClauseResult(assignArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(callArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesC8X, pqlUsesC10X, pqlUsesC13X };
		expectedClauseResult = RelationshipClauseResult(callArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("String literal - empty results") {
		expectedRetrievedFromPkb = {};

		clause = UsesSClause(whileArg, yLiteralArg);
		expectedClauseResult = RelationshipClauseResult(whileArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesSClause(ifArg, zLiteralArg);
		expectedClauseResult = RelationshipClauseResult(ifArg, zLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line Number and string literal - non empty results") {
		clause = UsesSClause(lineSixArg, yLiteralArg);
		expectedRetrievedFromPkb = { pqlUsesIf6Y };
		expectedClauseResult = RelationshipClauseResult(lineSixArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Line Number and string literal - empty results") {
		expectedRetrievedFromPkb = {};

		clause = UsesSClause(lineTwoArg, xLiteralArg);
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}