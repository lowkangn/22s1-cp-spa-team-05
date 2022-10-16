#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesPClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		ModifiesPClause modifiesPClause = ModifiesPClause(lhs, rhs);

		// when
		bool actual = modifiesPClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument secondProcArg = ClauseArgument::createProcedureArg("sp1");
	ClauseArgument firstVarArg = ClauseArgument::createVariableArg("v");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> modifiesPClauseAgain(new ModifiesPClause(firstProcArg, firstVarArg));
		testEquals(modifiesPClauseAgain, true);
	}

	SECTION("Not equal, same types different identifiers") {
		shared_ptr<RelationshipClause> modifiesPClauseDiffLhsString(new ModifiesPClause(secondProcArg, firstVarArg));
		testEquals(modifiesPClauseDiffLhsString, false);

		shared_ptr<RelationshipClause> modifiesPClauseDiffRhsString(new ModifiesPClause(firstProcArg, secondVarArg));
		testEquals(modifiesPClauseDiffRhsString, false);

		shared_ptr<RelationshipClause> modifiesPClauseDiffStrings(new ModifiesPClause(secondProcArg, secondVarArg));
		testEquals(modifiesPClauseDiffStrings, false);
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

	list<ClauseArgument> otherArguments = list<ClauseArgument>{stmtArg, readArg, printArg, assignArg, callArg,
															   whileArg, ifArg, constArg, lineNumArg, stringLitArg,
															   wildcardArg};

	SECTION("Not equal, different types") {
		for (ClauseArgument argument: otherArguments) {
			shared_ptr<RelationshipClause> modifiesPClauseDiffLhsArg(new ModifiesPClause(firstProcArg, argument));
			testEquals(modifiesPClauseDiffLhsArg, false);

			shared_ptr<RelationshipClause> modifiesPClauseDiffRhsArg(new ModifiesPClause(argument, firstVarArg));
			testEquals(modifiesPClauseDiffRhsArg, false);
		}
	}

	SECTION("Not even a ModifiesPClause") {
		shared_ptr<RelationshipClause> modifiesPClause(new ModifiesSClause(stmtArg, firstVarArg));
		testEquals(modifiesPClause, false);
	}
}

// =============== INTEGRATION TESTS ====================
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
			1. read y;
			2. x = y;
			3. while (x > 0) {
			4.	   call noModifies;
			5.     x = x - 1;
			   }
			6. if (y < 5) then {
			7.     y = 3;
			   } else {
			8.     call modifiesXAndY;
			   }
		}

		procedure test {
			9.	z = 2;
			10. call modifiesXAndY;
		}

		procedure noModifies {
			11. print z;
		}

		procedure modifiesXAndY {
			12. read x;
			13. call modifiesY;
		}

		procedure modifiesY {
			14. y = x;
		}
	*/

	// Initialise tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token testToken = Token::createNameOrKeywordToken("test");
	Token noModifiesToken = Token::createNameOrKeywordToken("noModifies");
	Token modifiesXYToken = Token::createNameOrKeywordToken("modifiesXAndY");
	Token modifiesYToken = Token::createNameOrKeywordToken("modifiesY");
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
	Entity noModifiesEntity = Entity::createProcedureEntity(noModifiesToken);
	Entity modifiesXYEntity = Entity::createProcedureEntity(modifiesXYToken);
	Entity modifiesYEntity = Entity::createProcedureEntity(modifiesYToken);

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

	Entity printEntity = Entity::createPrintEntity(11);
	Entity z11Entity = Entity::createVariableEntity(11, zToken);

	Entity read12Entity = Entity::createReadEntity(12);
	Entity x12Entity = Entity::createVariableEntity(12, xToken);

	Entity call13Entity = Entity::createCallEntity(13);

	Entity a14Entity = Entity::createAssignEntity(14);
	Entity y14Entity = Entity::createVariableEntity(14, yToken);
	Entity x14Entity = Entity::createVariableEntity(14, xToken);

	// Initialise modifies relationships
	Relationship modifiesMainY1 = Relationship::createModifiesRelationship(mainEntity, y1Entity);
	Relationship modifiesMainX2 = Relationship::createModifiesRelationship(mainEntity, x2Entity);
	Relationship modifiesMainX5 = Relationship::createModifiesRelationship(mainEntity, x5LhsEntity);
	Relationship modifiesMainY7 = Relationship::createModifiesRelationship(mainEntity, y7Entity);
	Relationship modifiesMainX12 = Relationship::createModifiesRelationship(mainEntity, x12Entity);
	Relationship modifiesMainY14 = Relationship::createModifiesRelationship(mainEntity, y14Entity);

	Relationship modifiesRead1Y1 = Relationship::createModifiesRelationship(read1Entity, y1Entity);
	Relationship modifiesA2X2 = Relationship::createModifiesRelationship(a2Entity, x2Entity);

	Relationship modifiesW3X5 = Relationship::createModifiesRelationship(whileEntity, x5LhsEntity);
	Relationship modifiesA5X5 = Relationship::createModifiesRelationship(a5Entity, x5RhsEntity);

	Relationship modifiesIf6Y7 = Relationship::createModifiesRelationship(ifEntity, y7Entity);
	Relationship modifiesIfX12 = Relationship::createModifiesRelationship(ifEntity, x12Entity);
	Relationship modifiesIfY14 = Relationship::createModifiesRelationship(ifEntity, y14Entity);
	Relationship modifiesA7Y7 = Relationship::createModifiesRelationship(a7Entity, y7Entity);
	Relationship modifiesCall8X12 = Relationship::createModifiesRelationship(call8Entity, x12Entity);
	Relationship modifiesCall8Y14 = Relationship::createModifiesRelationship(call8Entity, y14Entity);

	Relationship modifiesTestZ9 = Relationship::createModifiesRelationship(testEntity, z9Entity);
	Relationship modifiesTestX12 = Relationship::createModifiesRelationship(testEntity, x12Entity);
	Relationship modifiesTestY14 = Relationship::createModifiesRelationship(testEntity, y14Entity);

	Relationship modifiesA9Z9 = Relationship::createModifiesRelationship(a9Entity, z9Entity);
	Relationship modifiesCall10X12 = Relationship::createModifiesRelationship(call10Entity, x12Entity);
	Relationship modifiesCall10Y14 = Relationship::createModifiesRelationship(call10Entity, y14Entity);

	Relationship modifiesModifiesXYX12 = Relationship::createModifiesRelationship(modifiesXYEntity, x12Entity);
	Relationship modifiesModifiesXYY14 = Relationship::createModifiesRelationship(modifiesXYEntity, y14Entity);

	Relationship modifiesRead12X12 = Relationship::createModifiesRelationship(read12Entity, x12Entity);
	Relationship modifiesCall13Y14 = Relationship::createModifiesRelationship(call13Entity, y14Entity);

	Relationship modifiesModifiesYY14 = Relationship::createModifiesRelationship(modifiesYEntity, y14Entity);

	Relationship modifiesA14Y14 = Relationship::createModifiesRelationship(a14Entity, y14Entity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlTest = PQLEntity::generateProcedure("test");
	PQLEntity pqlModifiesXY = PQLEntity::generateProcedure("modifiesXAndY");
	PQLEntity pqlModifiesY = PQLEntity::generateProcedure("modifiesY");

	PQLEntity pqlVarX = PQLEntity::generateVariable("x");
	PQLEntity pqlVarY = PQLEntity::generateVariable("y");
	PQLEntity pqlVarZ = PQLEntity::generateVariable("z");

	PQLRelationship pqlModifiesMainY = PQLRelationship(pqlMain, pqlVarY);
	PQLRelationship pqlModifiesMainX = PQLRelationship(pqlMain, pqlVarX);

	PQLRelationship pqlModifiesTestZ = PQLRelationship(pqlTest, pqlVarZ);
	PQLRelationship pqlModifiesTestY = PQLRelationship(pqlTest, pqlVarY);
	PQLRelationship pqlModifiesTestX = PQLRelationship(pqlTest, pqlVarX);

	PQLRelationship pqlModifiesModifiesXYY = PQLRelationship(pqlModifiesXY, pqlVarY);
	PQLRelationship pqlModifiesModifiesXYX = PQLRelationship(pqlModifiesXY, pqlVarX);

	PQLRelationship pqlModifiesModifiesYY = PQLRelationship(pqlModifiesY, pqlVarY);

	// Clause Arguments
	ClauseArgument procedureArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument mainLiteralArg = ClauseArgument::createStringLiteralArg("main");
	ClauseArgument testLiteralArg = ClauseArgument::createStringLiteralArg("test");
	ClauseArgument noModifiesLiteralArg = ClauseArgument::createStringLiteralArg("noModifies");
	ClauseArgument modifiesYArg = ClauseArgument::createStringLiteralArg("modifiesY");
	ClauseArgument xLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument yLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument zLiteralArg = ClauseArgument::createStringLiteralArg("z");
	ClauseArgument aLiteralArg = ClauseArgument::createStringLiteralArg("a");
};

TEST_CASE("ModifiesPClause: test execute") {
	auto testExecute = [](ModifiesPClause modifiesPClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = modifiesPClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());

	vector<Entity> entities{ mainEntity, testEntity, noModifiesEntity, modifiesXYEntity, modifiesYEntity, read1Entity, y1Entity,
			a2Entity, x2Entity, y2Entity, whileEntity, xCondEntity, zeroConstEntity, call4Entity, a5Entity, x5LhsEntity,
			x5RhsEntity, oneConstEntity, ifEntity, yCondEntity, fiveConstEntity, a7Entity, y7Entity, threeConstEntity, call8Entity,
			a9Entity, z9Entity, twoConstEntity, call10Entity, printEntity, z11Entity, read12Entity, x12Entity, call13Entity,
			a14Entity, y14Entity, x14Entity };

	vector<Relationship> relationships{ modifiesMainY1 ,modifiesMainX2, modifiesMainX5, modifiesMainY7, modifiesMainX12,
			modifiesMainY14, modifiesRead1Y1, modifiesA2X2, modifiesW3X5, modifiesA5X5, modifiesIf6Y7, modifiesIfX12,
			modifiesIfY14, modifiesA7Y7, modifiesCall8X12, modifiesCall8Y14, modifiesTestZ9, modifiesTestX12, modifiesTestY14,
			modifiesA9Z9, modifiesCall10X12, modifiesCall10Y14, modifiesModifiesXYX12, modifiesModifiesXYY14, modifiesRead12X12,
			modifiesCall13Y14, modifiesModifiesYY14, modifiesA14Y14, modifiesMainY1, modifiesMainX2, modifiesMainX5,
			modifiesMainY7, modifiesMainX12, modifiesMainY14, modifiesRead1Y1, modifiesA2X2, modifiesW3X5, modifiesA5X5,
			modifiesIf6Y7, modifiesIfX12, modifiesIfY14, modifiesA7Y7, modifiesCall8X12, modifiesCall8Y14, modifiesTestZ9,
			modifiesTestX12, modifiesTestY14, modifiesA9Z9, modifiesCall10X12, modifiesCall10Y14, modifiesModifiesXYX12,
			modifiesModifiesXYY14, modifiesRead12X12, modifiesCall13Y14, modifiesModifiesYY14, modifiesA14Y14 };

	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	ModifiesPClause clause = ModifiesPClause(procedureArg, variableArg);

	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlModifiesMainY, pqlModifiesMainX, pqlModifiesTestZ, pqlModifiesTestY,
		pqlModifiesTestX, pqlModifiesModifiesXYY, pqlModifiesModifiesXYX, pqlModifiesModifiesYY };

	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(procedureArg, variableArg, expectedRetrievedFromPkb);

	SECTION("One procedure synonym and one variable synonym - all statement modifies") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure string literal - non empty results") {
		clause = ModifiesPClause(mainLiteralArg, variableArg);
		expectedRetrievedFromPkb = { pqlModifiesMainY, pqlModifiesMainX };
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ModifiesPClause(testLiteralArg, variableArg);
		expectedRetrievedFromPkb = { pqlModifiesTestZ, pqlModifiesTestY, pqlModifiesTestX };
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure string literal - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ModifiesPClause(noModifiesLiteralArg, variableArg);
		expectedClauseResult = RelationshipClauseResult(noModifiesLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Variable string literal - non empty results") {
		clause = ModifiesPClause(procedureArg, yLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesMainY, pqlModifiesTestY, pqlModifiesModifiesXYY, pqlModifiesModifiesYY };
		expectedClauseResult = RelationshipClauseResult(procedureArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ModifiesPClause(procedureArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesMainX, pqlModifiesTestX, pqlModifiesModifiesXYX };
		expectedClauseResult = RelationshipClauseResult(procedureArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Variable string literal - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ModifiesPClause(procedureArg, aLiteralArg);
		expectedClauseResult = RelationshipClauseResult(procedureArg, aLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure and variable string literals - non empty results") {
		clause = ModifiesPClause(mainLiteralArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesMainX };
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ModifiesPClause(testLiteralArg, zLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesTestZ };
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, zLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure and variable string literals - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ModifiesPClause(mainLiteralArg, zLiteralArg);
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, zLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ModifiesPClause(modifiesYArg, xLiteralArg);
		expectedClauseResult = RelationshipClauseResult(modifiesYArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}
