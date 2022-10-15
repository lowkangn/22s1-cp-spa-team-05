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
			4.	   print z;
			5.     x = x - 1;
			   }
		}

		procedure test {
			6. print b;
			7. x = y;
		}

		procedure noUses {
			8. print y;
		}
	*/

	// Initialise tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token testToken = Token::createNameOrKeywordToken("test");
	Token noUsesToken = Token::createNameOrKeywordToken("noUses");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token xToken = Token::createNameOrKeywordToken("x");

	// Initialise entities
	Entity mainEntity = Entity::createProcedureEntity(mainToken);
	Entity testEntity = Entity::createProcedureEntity(testToken);

	Entity y1Entity = Entity::createVariableEntity(1, yToken);
	Entity x2Entity = Entity::createVariableEntity(2, xToken);
	Entity x5LhsEntity = Entity::createVariableEntity(5, xToken);
	Entity x7Entity = Entity::createVariableEntity(7, xToken);

	// Initialise modifies relationships
	Relationship modifiesMainY1 = Relationship::createModifiesRelationship(mainEntity, y1Entity);
	Relationship modifiesMainX2 = Relationship::createModifiesRelationship(mainEntity, x2Entity);
	Relationship modifiesMainX5 = Relationship::createModifiesRelationship(mainEntity, x5LhsEntity);

	Relationship modifiesTestX7 = Relationship::createModifiesRelationship(testEntity, x7Entity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlTest = PQLEntity::generateProcedure("test");

	PQLEntity pqlVarX = PQLEntity::generateVariable("x");
	PQLEntity pqlVarY = PQLEntity::generateVariable("y");

	PQLRelationship pqlModifiesMainY = PQLRelationship(pqlMain, pqlVarY);
	PQLRelationship pqlModifiesMainX = PQLRelationship(pqlMain, pqlVarX);

	PQLRelationship pqlModifiesTestX = PQLRelationship(pqlTest, pqlVarX);

	// Clause Arguments
	ClauseArgument procedureArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument variableArg = ClauseArgument::createVariableArg("v");
	ClauseArgument mainLiteralArg = ClauseArgument::createStringLiteralArg("main");
	ClauseArgument testLiteralArg = ClauseArgument::createStringLiteralArg("test");
	ClauseArgument noUsesLiteralArg = ClauseArgument::createStringLiteralArg("noUses");
	ClauseArgument xLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument yLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument zLiteralArg = ClauseArgument::createStringLiteralArg("z");
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

	vector<Entity> entities{ mainEntity, testEntity, y1Entity, x2Entity, x5LhsEntity, x7Entity };

	vector<Relationship> relationships{ modifiesMainY1, modifiesMainX2, modifiesMainX5, modifiesTestX7 };

	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	ModifiesPClause clause = ModifiesPClause(procedureArg, variableArg);

	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlModifiesMainY, pqlModifiesMainX, pqlModifiesTestX };

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
		expectedRetrievedFromPkb = { pqlModifiesTestX };
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure string literal - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ModifiesPClause(noUsesLiteralArg, variableArg);
		expectedClauseResult = RelationshipClauseResult(noUsesLiteralArg, variableArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Variable string literal - non empty results") {
		clause = ModifiesPClause(procedureArg, yLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesMainY };
		expectedClauseResult = RelationshipClauseResult(procedureArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ModifiesPClause(procedureArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesMainX, pqlModifiesTestX };
		expectedClauseResult = RelationshipClauseResult(procedureArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Variable string literal - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ModifiesPClause(procedureArg, zLiteralArg);
		expectedClauseResult = RelationshipClauseResult(procedureArg, zLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure and variable string literals - non empty results") {
		clause = ModifiesPClause(mainLiteralArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesMainX };
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ModifiesPClause(testLiteralArg, xLiteralArg);
		expectedRetrievedFromPkb = { pqlModifiesTestX };
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, xLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Procedure and variable string literals - empty results") {
		expectedRetrievedFromPkb = {};

		clause = ModifiesPClause(mainLiteralArg, zLiteralArg);
		expectedClauseResult = RelationshipClauseResult(mainLiteralArg, zLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = ModifiesPClause(testLiteralArg, yLiteralArg);
		expectedClauseResult = RelationshipClauseResult(testLiteralArg, yLiteralArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}
