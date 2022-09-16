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
			4.	   print z;
			5.     x = x - 1;
			   }
			6. if (y < 5) then {
			7.     y = 3;
			   } else {
			8.     y = z;
			   }
	*/

	// Initialise tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token zeroToken = Token::createIntegerToken("0");
	Token oneToken = Token::createIntegerToken("1");
	Token threeToken = Token::createIntegerToken("3");
	Token fiveToken = Token::createIntegerToken("5");

	// Initialise entities
	Entity mainEntity = Entity::createProcedureEntity(mainToken);

	Entity readEntity = Entity::createReadEntity(1);
	Entity y1Entity = Entity::createVariableEntity(1, yToken);

	Entity a2Entity = Entity::createAssignEntity(2);
	Entity x2Entity = Entity::createVariableEntity(2, xToken);
	Entity y2Entity = Entity::createVariableEntity(2, yToken);

	Entity whileEntity = Entity::createWhileEntity(3);
	Entity xCondEntity = Entity::createVariableEntity(3, xToken);
	Entity zeroConstEntity = Entity::createConstantEntity(3, zeroToken);

	Entity printEntity = Entity::createPrintEntity(4);
	Entity z4Entity = Entity::createVariableEntity(4, zToken);

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

	Entity a8Entity = Entity::createAssignEntity(8);
	Entity y8Entity = Entity::createVariableEntity(8, yToken);
	Entity z8Entity = Entity::createVariableEntity(8, zToken);

	// Initialise uses relationships
	Relationship usesMainY2 = Relationship::createUsesRelationship(mainEntity, y2Entity);
	Relationship usesMainXCond = Relationship::createUsesRelationship(mainEntity, xCondEntity);
	Relationship usesMainZ4 = Relationship::createUsesRelationship(mainEntity, z4Entity);
	Relationship usesMainX5 = Relationship::createUsesRelationship(mainEntity, x5RhsEntity);
	Relationship usesMainYCond = Relationship::createUsesRelationship(mainEntity, yCondEntity);
	Relationship usesMainZ8 = Relationship::createUsesRelationship(mainEntity, z8Entity);

	Relationship usesA2Y2 = Relationship::createUsesRelationship(a2Entity, y2Entity);

	Relationship usesW3XCond = Relationship::createUsesRelationship(whileEntity, xCondEntity);
	Relationship usesW3Z4 = Relationship::createUsesRelationship(whileEntity, z4Entity);
	Relationship usesW3X5 = Relationship::createUsesRelationship(whileEntity, x5RhsEntity);

	Relationship usesIf6YCond = Relationship::createUsesRelationship(ifEntity, yCondEntity);
	Relationship usesIf6Z8 = Relationship::createUsesRelationship(ifEntity, z8Entity);

	Relationship usesP4Z4 = Relationship::createUsesRelationship(printEntity, z4Entity);
	Relationship usesA5X5 = Relationship::createUsesRelationship(a5Entity, x5RhsEntity);
	Relationship usesA8Z8 = Relationship::createUsesRelationship(a8Entity, z8Entity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlR1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlP4 = PQLEntity::generateStatement(4);
	PQLEntity pqlA5 = PQLEntity::generateStatement(5);
	PQLEntity pqlI6 = PQLEntity::generateStatement(6);
	PQLEntity pqlA7 = PQLEntity::generateStatement(7);
	PQLEntity pqlA8 = PQLEntity::generateStatement(8);

	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlVarX = PQLEntity::generateVariable("x");
	PQLEntity pqlVarY = PQLEntity::generateVariable("y");
	PQLEntity pqlVarZ = PQLEntity::generateVariable("z");

	PQLRelationship pqlUsesMainY2 = PQLRelationship(pqlMain, pqlVarY);
	PQLRelationship pqlUsesMainXCond = PQLRelationship(pqlMain, pqlVarX);
	PQLRelationship pqlUsesMainZ4 = PQLRelationship(pqlMain, pqlVarZ);
	PQLRelationship pqlUsesMainX5 = PQLRelationship(pqlMain, pqlVarX);
	PQLRelationship pqlUsesMainYCond = PQLRelationship(pqlMain, pqlVarY);
	PQLRelationship pqlUsesMainZ8 = PQLRelationship(pqlMain, pqlVarZ);

	PQLRelationship pqlUsesA2Y2 = PQLRelationship(pqlA2, pqlVarY);

	PQLRelationship pqlUsesW3XCond = PQLRelationship(pqlW3, pqlVarX);
	PQLRelationship pqlUsesW3Z4 = PQLRelationship(pqlW3, pqlVarZ);
	PQLRelationship pqlUsesW3X5 = PQLRelationship(pqlW3, pqlVarX);

	PQLRelationship pqlUsesIf6YCond = PQLRelationship(pqlI6, pqlVarY);
	PQLRelationship pqlUsesIf6Z8 = PQLRelationship(pqlI6, pqlVarZ);

	PQLRelationship pqlUsesP4Z4 = PQLRelationship(pqlP4, pqlVarZ);
	PQLRelationship pqlUsesA5X5 = PQLRelationship(pqlA5, pqlVarX);
	PQLRelationship pqlUsesA8Z8 = PQLRelationship(pqlA8, pqlVarZ);

	// Clause Arguments
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument ifArg = ClauseArgument::createIfArg("if");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("while");
	ClauseArgument lineOneArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
	ClauseArgument readArg = ClauseArgument::createReadArg("rr");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");
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

	vector<Entity> entities{ mainEntity, readEntity, y1Entity, a2Entity, x2Entity, y2Entity, whileEntity, xCondEntity, 
		zeroConstEntity, printEntity, z4Entity, a5Entity, x5LhsEntity, x5RhsEntity, oneConstEntity, ifEntity, 
		yCondEntity, fiveConstEntity, a7Entity, y7Entity, threeConstEntity, a8Entity, y8Entity, z8Entity };

	vector<Relationship> relationships{ usesMainY2, usesMainXCond, usesMainZ4, usesMainX5, usesMainYCond, usesMainZ8, 
		usesA2Y2, usesW3XCond, usesW3Z4, usesW3X5, usesIf6YCond, usesIf6Z8, usesP4Z4, usesA5X5, usesA8Z8 };

	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	UsesSClause clause = UsesSClause(stmtArg, variableArg);

	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlUsesA2Y2, pqlUsesW3XCond, pqlUsesW3Z4, pqlUsesW3X5, 
		pqlUsesIf6YCond, pqlUsesIf6Z8, pqlUsesP4Z4, pqlUsesA5X5,pqlUsesA8Z8 };

	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(stmtArg, variableArg,
		expectedRetrievedFromPkb);

	SECTION("Two stmt synoynms - all statement uses") {
		testExecute(clause, expectedClauseResult, pkb);
	}
}