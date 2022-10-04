#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectClause: test equals") {
	auto testEquals = [](SelectClause first, const SelectClause* second, bool expected) {
		// when
		bool actual = first.equals(second);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument assignArgSameIdentifier = ClauseArgument::createAssignArg("v");
	ClauseArgument varArgDifferentIdentifier = ClauseArgument::createVariableArg("a");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");

	SelectClause varArgSelectClause = SelectClause::createSynonymSelectClause({varArg});
	SelectClause anotherVarArgSelectClause = SelectClause::createSynonymSelectClause({varArg});
	SelectClause assignArgSelectClause = SelectClause::createSynonymSelectClause({assignArgSameIdentifier});
	SelectClause differentVarArgSelectClause = SelectClause::createSynonymSelectClause({varArgDifferentIdentifier});

	SECTION("Single return value equal") {
		testEquals(varArgSelectClause, &varArgSelectClause, true);
		testEquals(varArgSelectClause, &anotherVarArgSelectClause, true);
	}

	SECTION("Single return value not equal") {
		testEquals(varArgSelectClause, &assignArgSelectClause, false);
		testEquals(varArgSelectClause, &differentVarArgSelectClause, false);
	}

	SelectClause booleanSelectClause = SelectClause::createBooleanSelectClause();
	SelectClause anotherBooleanSelectClause = SelectClause::createBooleanSelectClause();

	SECTION("Boolean return value equal") {
		testEquals(booleanSelectClause, &booleanSelectClause, true);
		testEquals(booleanSelectClause, &anotherBooleanSelectClause, true);
	}

	ClauseArgument callArg = ClauseArgument::createCallArg("c");

	SelectClause varAndAssignArgsSelectClause = SelectClause::createSynonymSelectClause({varArg, assignArg});
	SelectClause anotherVarAndAssignArgsSelectClause = SelectClause::createSynonymSelectClause({varArg, assignArg});

	SelectClause varAndCallArgsSelectClause = SelectClause::createSynonymSelectClause({varArg, callArg});
	SelectClause assignAndCallArgsSelectClause = SelectClause::createSynonymSelectClause({assignArg, callArg});

	SECTION("Multiple return values equal") {
		testEquals(varAndAssignArgsSelectClause, &varAndAssignArgsSelectClause, true);
		testEquals(varAndAssignArgsSelectClause, &anotherVarAndAssignArgsSelectClause, true);
	}

	SECTION("Multiple return values not equal") {
		testEquals(varAndAssignArgsSelectClause, &varAndCallArgsSelectClause, false);
		testEquals(varAndAssignArgsSelectClause, &assignAndCallArgsSelectClause, false);
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
			5.	   if (y > x) {
			6.     	    x = x - 1;
				   }
			   }
			7. if (y < 5) then {
			8.     y = 3;
			   } else {
			9.     y = z;
			   }
		}
	*/

	// Initialise statements
	Entity readEntity = Entity::createReadEntity(1);
	Entity a2Entity = Entity::createAssignEntity(2);
	Entity whileEntity = Entity::createWhileEntity(3);
	Entity printEntity = Entity::createPrintEntity(4);
	Entity if5Entity = Entity::createIfEntity(5);
	Entity a6Entity = Entity::createAssignEntity(6);
	Entity if7Entity = Entity::createIfEntity(7);
	Entity a8Entity = Entity::createAssignEntity(8);
	Entity a9Entity = Entity::createAssignEntity(9);

	// Initialise entities
	Entity main = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
	Entity y = Entity::createVariableEntity(1, Token::createNameOrKeywordToken("y"));
	Entity x = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x"));
	Entity zeroEntity = Entity::createConstantEntity(3, Token::createIntegerToken("0"));
	Entity z = Entity::createVariableEntity(4, Token::createNameOrKeywordToken("z"));
	Entity oneEntity = Entity::createConstantEntity(6, Token::createIntegerToken("1"));
	Entity fiveEntity = Entity::createConstantEntity(7, Token::createIntegerToken("5"));
	Entity threeEntity = Entity::createConstantEntity(8, Token::createIntegerToken("3"));

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlR1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlW3 = PQLEntity::generateStatement(3);
	PQLEntity pqlP4 = PQLEntity::generateStatement(4);
	PQLEntity pqlI5 = PQLEntity::generateStatement(5);
	PQLEntity pqlA6 = PQLEntity::generateStatement(6);
	PQLEntity pqlI7 = PQLEntity::generateStatement(7);
	PQLEntity pqlA8 = PQLEntity::generateStatement(8);
	PQLEntity pqlA9 = PQLEntity::generateStatement(9);

	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlX = PQLEntity::generateVariable("x");
	PQLEntity pqlY = PQLEntity::generateVariable("y");
	PQLEntity pqlZ = PQLEntity::generateVariable("z");
	PQLEntity pql0 = PQLEntity::generateConstant(0);
	PQLEntity pql1 = PQLEntity::generateConstant(1);
	PQLEntity pql3 = PQLEntity::generateConstant(3);
	PQLEntity pql5 = PQLEntity::generateConstant(5);

	// Clause Arguments
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	ClauseArgument anotherAssignArg = ClauseArgument::createAssignArg("a1");

}

TEST_CASE("SelectClause: test execute") {
	auto testExecute = [](SelectClause selectClause,
			set<EntityClauseResult> expectedSet, shared_ptr<PKB> pkb) {
		// given
		shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

		// when
		list<shared_ptr<EntityClauseResult>> actualList = selectClause.execute(pkb);

		// then
		set<EntityClauseResult> actualSet;
		for (shared_ptr<EntityClauseResult> actualResult : actualList) {
			actualSet.insert(*actualResult);
		}

		REQUIRE(actualSet == expectedSet);
	};

	// ------ PKB ------
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ readEntity, a2Entity, whileEntity, printEntity, if5Entity, a6Entity, if7Entity, a8Entity, a9Entity,
							 main, x, y, z, zeroEntity, oneEntity, threeEntity, fiveEntity };

	// ------ PKB ------
	pkb->addEntities(entities);

	// ------ QPS ------
	SelectClause selectClause = SelectClause::createBooleanSelectClause();
	set<EntityClauseResult> expectedSet = {};

	SECTION("Boolean return type") {
		testExecute(selectClause, expectedSet, pkb);
	}

	SECTION("Single return type") {
		selectClause = SelectClause::createSynonymSelectClause({stmtArg});
		EntityClauseResult result = EntityClauseResult(stmtArg,vector<PQLEntity>{
			pqlR1, pqlA2, pqlW3, pqlP4, pqlI5, pqlA6, pqlI7, pqlA8, pqlA9});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({assignArg});
		result = EntityClauseResult(assignArg,vector<PQLEntity>{
				pqlA2, pqlA6, pqlA8, pqlA9});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({readArg});
		result = EntityClauseResult(readArg,vector<PQLEntity>{pqlR1});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({printArg});
		result = EntityClauseResult(printArg,vector<PQLEntity>{{pqlP4}});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({ifArg});
		result = EntityClauseResult(ifArg,vector<PQLEntity>{pqlI5, pqlI7});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({whileArg});
		result = EntityClauseResult(whileArg,vector<PQLEntity>{pqlW3});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({procArg});
		result = EntityClauseResult(procArg,vector<PQLEntity>{pqlMain});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({varArg});
		result = EntityClauseResult(varArg,vector<PQLEntity>{pqlX, pqlY, pqlZ});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({constArg});
		result = EntityClauseResult(constArg,vector<PQLEntity>{pql0, pql1, pql3, pql5});
		expectedSet = {result};
		testExecute(selectClause, expectedSet, pkb);
	}

	SECTION("Multiple return types") {
		selectClause = SelectClause::createSynonymSelectClause({assignArg, readArg});
		EntityClauseResult assignResult = EntityClauseResult(assignArg,vector<PQLEntity>{
				pqlA2, pqlA6, pqlA8, pqlA9});
		EntityClauseResult readResult = EntityClauseResult(readArg,vector<PQLEntity>{pqlR1});
		expectedSet = {assignResult, readResult};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({assignArg, varArg});
		EntityClauseResult varResult = EntityClauseResult(varArg,vector<PQLEntity>{pqlX, pqlY, pqlZ});
		expectedSet = {assignResult, varResult};
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({assignArg, anotherAssignArg});
		EntityClauseResult anotherAssignResult = EntityClauseResult(anotherAssignArg,vector<PQLEntity>{
				pqlA2, pqlA6, pqlA8, pqlA9});
		expectedSet = {assignResult, anotherAssignResult};
		testExecute(selectClause, expectedSet, pkb);
	}
}
