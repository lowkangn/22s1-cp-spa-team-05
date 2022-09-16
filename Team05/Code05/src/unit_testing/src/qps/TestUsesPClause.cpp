#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/UsesPClause.h>
#include <qps/query/clause/UsesPClause.h>

#include <list>
#include <iostream>

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
			4.	   print z;
			5.     x = x - 1;
			   }
		}

		procedure test {
			6. print b;
			7. x = y;
		}
	*/

	// Initialise tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token testToken = Token::createNameOrKeywordToken("test");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token bToken = Token::createNameOrKeywordToken("b");
	Token zeroToken = Token::createIntegerToken("0");
	Token oneToken = Token::createIntegerToken("1");

	// Initialise entities
	Entity mainEntity = Entity::createProcedureEntity(mainToken);
	Entity testEntity = Entity::createProcedureEntity(testToken);

	Entity read1Entity = Entity::createReadEntity(1);
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

	Entity print6Entity = Entity::createPrintEntity(6);
	Entity b6Entity = Entity::createVariableEntity(6, bToken);

	Entity a7Entity = Entity::createAssignEntity(7);
	Entity x7Entity = Entity::createVariableEntity(7, xToken);
	Entity y7Entity = Entity::createVariableEntity(7, yToken);

	// Initialise uses relationships
	Relationship usesMainY2 = Relationship::createUsesRelationship(mainEntity, y2Entity);
	Relationship usesMainXCond = Relationship::createUsesRelationship(mainEntity, xCondEntity);
	Relationship usesMainZ4 = Relationship::createUsesRelationship(mainEntity, z4Entity);
	Relationship usesMainX5 = Relationship::createUsesRelationship(mainEntity, x5RhsEntity);

	Relationship usesTestB6 = Relationship::createUsesRelationship(testEntity, b6Entity);
	Relationship usesTestY7 = Relationship::createUsesRelationship(testEntity, y7Entity);

	Relationship usesA2Y2 = Relationship::createUsesRelationship(a2Entity, y2Entity);

	Relationship usesW3XCond = Relationship::createUsesRelationship(whileEntity, xCondEntity);
	Relationship usesW3Z4 = Relationship::createUsesRelationship(whileEntity, z4Entity);
	Relationship usesW3X5 = Relationship::createUsesRelationship(whileEntity, x5RhsEntity);

	Relationship usesP4Z4 = Relationship::createUsesRelationship(print4Entity, z4Entity);
	Relationship usesA5X5 = Relationship::createUsesRelationship(a5Entity, x5RhsEntity);
	Relationship usesP6B6 = Relationship::createUsesRelationship(print6Entity, b6Entity);
	Relationship usesA7Y7 = Relationship::createUsesRelationship(a7Entity, y7Entity);


	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlTest = PQLEntity::generateProcedure("test");

	PQLEntity pqlVarX = PQLEntity::generateVariable("x");
	PQLEntity pqlVarY = PQLEntity::generateVariable("y");
	PQLEntity pqlVarZ = PQLEntity::generateVariable("z");
	PQLEntity pqlVarB = PQLEntity::generateVariable("b");

	PQLRelationship pqlUsesMainY = PQLRelationship(pqlMain, pqlVarY);
	PQLRelationship pqlUsesMainX = PQLRelationship(pqlMain, pqlVarX);
	PQLRelationship pqlUsesMainZ = PQLRelationship(pqlMain, pqlVarZ);

	PQLRelationship pqlUsesTestB = PQLRelationship(pqlTest, pqlVarB);
	PQLRelationship pqlUsesTestY = PQLRelationship(pqlTest, pqlVarY);

	// Clause Arguments
	ClauseArgument procedureArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument mainLiteralArg = ClauseArgument::createStringLiteralArg("main");
	ClauseArgument testLiteralArg = ClauseArgument::createStringLiteralArg("test");
	ClauseArgument xLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument yLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument aLiteralArg = ClauseArgument::createStringLiteralArg("a");
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

	vector<Entity> entities{ mainEntity, testEntity, read1Entity, y1Entity, a2Entity, x2Entity, y2Entity, 
		whileEntity, xCondEntity, zeroConstEntity, print4Entity, z4Entity, a5Entity, x5LhsEntity, x5RhsEntity, 
		oneConstEntity, print6Entity, b6Entity, a7Entity, x7Entity, y7Entity };

	vector<Relationship> relationships{ usesMainY2,usesMainXCond, usesMainZ4, usesMainX5, usesTestB6, usesTestY7, usesA2Y2, 
		usesW3XCond, usesW3Z4, usesW3X5, usesP4Z4, usesA5X5, usesP6B6, usesA7Y7 };

	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	UsesPClause clause = UsesPClause(procedureArg, variableArg);

	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlUsesMainY, pqlUsesMainX, pqlUsesMainZ, pqlUsesTestB, pqlUsesTestY };

	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(procedureArg, variableArg, expectedRetrievedFromPkb);

	SECTION("One procedure synonym and one variable synonym - all statement uses") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure string literal - non empty results") {
		clause = UsesPClause(mainLiteralArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesMainY, pqlUsesMainX, pqlUsesMainZ };
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = UsesPClause(testLiteralArg, variableArg);
		expectedRetrievedFromPkb = { pqlUsesTestB, pqlUsesTestY };
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, variableArg, expectedRetrievedFromPkb);
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
}
