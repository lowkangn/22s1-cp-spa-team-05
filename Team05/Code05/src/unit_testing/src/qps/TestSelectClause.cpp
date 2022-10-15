#include "catch.hpp"
#include <qps/query/clause/SelectClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectClause: test equals") {
	auto testEquals = [](shared_ptr<SelectClause> first, shared_ptr<SelectClause> second, bool expected) {
		// when
		bool actual = first->equals(second);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument assignArgSameIdentifier = ClauseArgument::createAssignArg("v");
	ClauseArgument varArgDifferentIdentifier = ClauseArgument::createVariableArg("a");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");

	shared_ptr<SelectClause> varArgSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
	shared_ptr<SelectClause> anotherVarArgSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
	shared_ptr<SelectClause> assignArgSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArgSameIdentifier}));
	shared_ptr<SelectClause> differentVarArgSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArgDifferentIdentifier}));

	SECTION("Single return value equal") {
		testEquals(varArgSelectClause, varArgSelectClause, true);
		testEquals(varArgSelectClause, anotherVarArgSelectClause, true);
	}

	SECTION("Single return value not equal") {
		testEquals(varArgSelectClause, assignArgSelectClause, false);
		testEquals(varArgSelectClause, differentVarArgSelectClause, false);
	}

	shared_ptr<SelectClause> booleanSelectClause = make_shared<SelectClause>(SelectClause::createBooleanSelectClause());
	shared_ptr<SelectClause> anotherBooleanSelectClause = make_shared<SelectClause>(SelectClause::createBooleanSelectClause());

	SECTION("Boolean return value equal") {
		testEquals(booleanSelectClause, booleanSelectClause, true);
		testEquals(booleanSelectClause, anotherBooleanSelectClause, true);
	}

	ClauseArgument callArg = ClauseArgument::createCallArg("c");

	shared_ptr<SelectClause> varAndAssignArgsSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg, assignArg}));
	shared_ptr<SelectClause> anotherVarAndAssignArgsSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg, assignArg}));

	shared_ptr<SelectClause> varAndCallArgsSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg, callArg}));
	shared_ptr<SelectClause> assignAndCallArgsSelectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg, callArg}));

	SECTION("Multiple return values equal") {
		testEquals(varAndAssignArgsSelectClause, varAndAssignArgsSelectClause, true);
		testEquals(varAndAssignArgsSelectClause, anotherVarAndAssignArgsSelectClause, true);
	}

	SECTION("Multiple return values not equal") {
		testEquals(varAndAssignArgsSelectClause, varAndCallArgsSelectClause, false);
		testEquals(varAndAssignArgsSelectClause, assignAndCallArgsSelectClause, false);
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
		procedure procedure {
			10. call main;
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
	Entity callEntity = Entity::createCallEntity(10);

	// Initialise entities
	Entity main = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
	Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("procedure"));
	Entity y = Entity::createVariableEntity(1, Token::createNameOrKeywordToken("y"));
	Entity x = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x"));
	Entity zeroEntity = Entity::createConstantEntity(3, Token::createIntegerToken("0"));
	Entity z4 = Entity::createVariableEntity(4, Token::createNameOrKeywordToken("z"));
	Entity z9 = Entity::createVariableEntity(9, Token::createNameOrKeywordToken("z"));
	Entity oneEntity = Entity::createConstantEntity(6, Token::createIntegerToken("1"));
	Entity fiveEntity = Entity::createConstantEntity(7, Token::createIntegerToken("5"));
	Entity threeEntity = Entity::createConstantEntity(8, Token::createIntegerToken("3"));

	// Initialise relationships
	Relationship modifiesR1Y = Relationship::createModifiesRelationship(readEntity, y);
	Relationship usesP4Z4 = Relationship::createUsesRelationship(printEntity, z4);
	Relationship callsAttributeC10Main = Relationship::createCallStmtAttributeRelationship(callEntity, main);
	// + Additional relationshps that should not get retrieved
	Relationship modifiesA2X = Relationship::createModifiesRelationship(a2Entity, x);
	Relationship usesIf7Z9 = Relationship::createUsesRelationship(if7Entity, z9);
	Relationship callsProcedureMain = Relationship::createCallsRelationship(procedure, main);

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
	PQLEntity pqlC10 = PQLEntity::generateStatement(10);

	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlProcedure = PQLEntity::generateProcedure("procedure");
	PQLEntity pqlX = PQLEntity::generateVariable("x");
	PQLEntity pqlY = PQLEntity::generateVariable("y");
	PQLEntity pqlZ = PQLEntity::generateVariable("z");
	PQLEntity pql0 = PQLEntity::generateConstant(0);
	PQLEntity pql1 = PQLEntity::generateConstant(1);
	PQLEntity pql3 = PQLEntity::generateConstant(3);
	PQLEntity pql5 = PQLEntity::generateConstant(5);

	PQLRelationship pqlModifiesR1Y = PQLRelationship(pqlR1, pqlY);
	PQLRelationship pqlUsesP4Z = PQLRelationship(pqlP4, pqlZ);
	PQLRelationship pqlCallsAttributeC10Main = PQLRelationship(pqlC10, pqlMain);

	// Clause Arguments
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument callArg = ClauseArgument::createCallArg("ca");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	ClauseArgument anotherAssignArg = ClauseArgument::createAssignArg("a1");

	ClauseArgument stmtStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(stmtArg);
	ClauseArgument assignStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(assignArg);
	ClauseArgument callStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(callArg);
	ClauseArgument readStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(readArg);
	ClauseArgument printStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(printArg);

	ClauseArgument callProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(callArg);
	ClauseArgument readVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(readArg);
	ClauseArgument printVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(printArg);

	ClauseArgument varVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(varArg);
	ClauseArgument constValueAttributeArg = ClauseArgument::createValueAttributeArg(constArg);
	ClauseArgument procProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(procArg);
}

TEST_CASE("SelectClause: test execute") {
	auto testExecute = [](SelectClause selectClause,
			set<ClauseResult> expectedSet, shared_ptr<PKB> pkb) {
		// given
		shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

		// when
		list<shared_ptr<ClauseResult>> actualList = selectClause.execute(pkb);

		// then
		set<ClauseResult> actualSet;
		for (shared_ptr<ClauseResult> actualResult : actualList) {
			actualSet.insert(*actualResult);
		}

		REQUIRE(actualSet == expectedSet);
	};

	// ------ PKB ------
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ readEntity, a2Entity, whileEntity, printEntity, if5Entity, a6Entity, if7Entity, a8Entity, a9Entity,
							 callEntity, main, procedure, x, y, z4, z9, zeroEntity, oneEntity, threeEntity, fiveEntity };
	vector<Relationship> relationships{ modifiesR1Y, usesP4Z4, callsAttributeC10Main, modifiesA2X, usesIf7Z9, callsProcedureMain};
	// ------ PKB ------
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);

	// ------ QPS ------
	SelectClause selectClause = SelectClause::createBooleanSelectClause();
	set<ClauseResult> expectedSet = {};

	SECTION("Boolean return type") {
		testExecute(selectClause, expectedSet, pkb);
	}

	SECTION("Single return type") {
		selectClause = SelectClause::createSynonymSelectClause({stmtArg});
		EntityClauseResult result = EntityClauseResult(stmtArg,vector<PQLEntity>{
			pqlR1, pqlA2, pqlW3, pqlP4, pqlI5, pqlA6, pqlI7, pqlA8, pqlA9, pqlC10});
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
		result = EntityClauseResult(procArg,vector<PQLEntity>{pqlMain, pqlProcedure});
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
		selectClause = SelectClause::createSynonymSelectClause({assignArg, readArg });
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

	SECTION("Select attributes") {
		selectClause = SelectClause::createSynonymSelectClause({ assignArg, assignStmtNumAttributeArg });
		EntityClauseResult assignStmtNumResult = EntityClauseResult(assignArg, vector<PQLEntity>{
			pqlA2, pqlA6, pqlA8, pqlA9});
		expectedSet = { assignStmtNumResult };
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({ constArg, constValueAttributeArg, readArg, readVarNameAttributeArg, readArg });
		EntityClauseResult constValueResult = EntityClauseResult(constArg, vector<PQLEntity>{
			pql0, pql1, pql3, pql5});
		RelationshipClauseResult readVarNameResult = RelationshipClauseResult(readArg, readVarNameAttributeArg,
			vector<PQLRelationship>{pqlModifiesR1Y});
		EntityClauseResult readResult = EntityClauseResult(readArg, vector<PQLEntity>{pqlR1});
		expectedSet = { constValueResult, readVarNameResult, readResult };
		testExecute(selectClause, expectedSet, pkb);

		selectClause = SelectClause::createSynonymSelectClause({ callArg, callProcNameAttributeArg, procArg, ifArg });
		RelationshipClauseResult callProcNameResult = RelationshipClauseResult(callArg, callProcNameAttributeArg,
			vector<PQLRelationship>{pqlCallsAttributeC10Main});
		EntityClauseResult procResult = EntityClauseResult(procArg, vector<PQLEntity>{pqlMain, pqlProcedure});
		EntityClauseResult ifResult = EntityClauseResult(ifArg, vector<PQLEntity>{pqlI5, pqlI7});
		expectedSet = { callProcNameResult, procResult, ifResult };
		testExecute(selectClause, expectedSet, pkb);
	}
}
