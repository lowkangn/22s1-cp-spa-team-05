#include "catch.hpp"
#include <qps/query_evaluator/QueryEvaluator.h>

#include <memory>

using namespace std;

// ==================== INTEGRATION TESTS ====================
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
	 	procedure x {
	 		10.	   call main;
	 	}
	 	procedure y {
	 		11.    call x;
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
	Entity c10Entity = Entity::createCallEntity(10);
	Entity c11Entity = Entity::createCallEntity(11);

	// Initialise entities
	Entity main = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
	Entity y = Entity::createVariableEntity(1, Token::createNameOrKeywordToken("y"));
	Entity x = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x"));
	Entity zeroEntity = Entity::createConstantEntity(3, Token::createIntegerToken("0"));
	Entity z = Entity::createVariableEntity(4, Token::createNameOrKeywordToken("z"));
	Entity oneEntity = Entity::createConstantEntity(6, Token::createIntegerToken("1"));
	Entity fiveEntity = Entity::createConstantEntity(7, Token::createIntegerToken("5"));
	Entity threeEntity = Entity::createConstantEntity(8, Token::createIntegerToken("3"));
	Entity procedureX = Entity::createProcedureEntity(Token::createNameOrKeywordToken("x"));
	Entity procedureY = Entity::createProcedureEntity(Token::createNameOrKeywordToken("y"));

	// Initialise follows relationships
	Relationship followsReadA2 = Relationship::createFollowsRelationship(readEntity, a2Entity);
	Relationship followsA2While = Relationship::createFollowsRelationship(a2Entity, whileEntity);
	Relationship followsPrintIf5 = Relationship::createFollowsRelationship(printEntity, if5Entity);
	Relationship followsWhileIf7 = Relationship::createFollowsRelationship(whileEntity, if7Entity);

	Relationship followsStarReadA2 = Relationship::createFollowsTRelationship(readEntity, a2Entity);
	Relationship followsStarReadWhile = Relationship::createFollowsTRelationship(readEntity, whileEntity);
	Relationship followsStarReadIf7 = Relationship::createFollowsTRelationship(readEntity, if7Entity);

	Relationship followsStarA2While = Relationship::createFollowsTRelationship(a2Entity, whileEntity);
	Relationship followsStarA2If7 = Relationship::createFollowsTRelationship(a2Entity, if7Entity);

	Relationship followsStarWhileIf7 = Relationship::createFollowsTRelationship(whileEntity, if7Entity);

	Relationship followsStarPrintIf5 = Relationship::createFollowsTRelationship(printEntity, if5Entity);

	// Initialise parent relationships
	Relationship parentWhilePrint = Relationship::createParentRelationship(whileEntity, printEntity);
	Relationship parentWhileIf5 = Relationship::createParentRelationship(whileEntity, if5Entity);
	Relationship parentIf5A6 = Relationship::createParentRelationship(if5Entity, a6Entity);
	Relationship parentIf7A8 = Relationship::createParentRelationship(if7Entity, a8Entity);
	Relationship parentIf7A9 = Relationship::createParentRelationship(if7Entity, a9Entity);

	Relationship parentStarWhilePrint = Relationship::createParentTRelationship(whileEntity, printEntity);
	Relationship parentStarWhileIf5 = Relationship::createParentTRelationship(whileEntity, if5Entity);
	Relationship parentStarWhileA6 = Relationship::createParentTRelationship(whileEntity, a6Entity);

	Relationship parentStarIf5A6 = Relationship::createParentTRelationship(if5Entity, a6Entity);

	Relationship parentStarIf7A8 = Relationship::createParentTRelationship(if7Entity, a8Entity);
	Relationship parentStarIf7A9 = Relationship::createParentTRelationship(if7Entity, a9Entity);

	// Initialise modifies relationships
	Relationship modifiesSReadY = Relationship::createModifiesRelationship(readEntity, y);
	Relationship modifiesSA2X = Relationship::createModifiesRelationship(a2Entity, x);
	Relationship modifiesSA6X = Relationship::createModifiesRelationship(a6Entity, x);
	Relationship modifiesSA8Y = Relationship::createModifiesRelationship(a8Entity, y);
	Relationship modifiesSA9Y = Relationship::createModifiesRelationship(a9Entity, y);

	Relationship modifiesSWhileX = Relationship::createModifiesRelationship(whileEntity, x);
	Relationship modifiesSIf5X = Relationship::createModifiesRelationship(if5Entity, x);
	Relationship modifiesSIf7Y = Relationship::createModifiesRelationship(if7Entity, y);

	Relationship modifiesPMainY = Relationship::createModifiesRelationship(main, y);
	Relationship modifiesPMainX = Relationship::createModifiesRelationship(main, x);

	Relationship modifiesPXY = Relationship::createModifiesRelationship(procedureX, y);
	Relationship modifiesPXX = Relationship::createModifiesRelationship(procedureX, x);

	Relationship modifiesPYY = Relationship::createModifiesRelationship(procedureY, y);
	Relationship modifiesPYX = Relationship::createModifiesRelationship(procedureY, x);

	// Initialise uses relationships
	Relationship usesSA2Y = Relationship::createUsesRelationship(a2Entity, y);
	Relationship usesSPrintZ = Relationship::createUsesRelationship(printEntity, z);
	Relationship usesSA6X = Relationship::createUsesRelationship(a6Entity, x);
	Relationship usesSA9Z = Relationship::createUsesRelationship(a9Entity, z);

	Relationship usesSWhileX = Relationship::createUsesRelationship(whileEntity, x);
	Relationship usesSIf5Y = Relationship::createUsesRelationship(if5Entity, y);
	Relationship usesSIf5X = Relationship::createUsesRelationship(if5Entity, x);
	Relationship usesSIf7Y = Relationship::createUsesRelationship(if7Entity, y);

	Relationship usesPMainY = Relationship::createUsesRelationship(main, y);
	Relationship usesPMainX = Relationship::createUsesRelationship(main, x);
	Relationship usesPMainZ = Relationship::createUsesRelationship(main, z);

	Relationship usesPXY = Relationship::createUsesRelationship(procedureX, y);
	Relationship usesPXX = Relationship::createUsesRelationship(procedureX, x);
	Relationship usesPXZ = Relationship::createUsesRelationship(procedureX, z);

	Relationship usesPYY = Relationship::createUsesRelationship(procedureY, y);
	Relationship usesPYX = Relationship::createUsesRelationship(procedureY, x);
	Relationship usesPYZ = Relationship::createUsesRelationship(procedureY, z);

	Relationship callsC10PMain = Relationship::createCallStmtAttributeRelationship(c10Entity, main);
	Relationship callsC11PX = Relationship::createCallStmtAttributeRelationship(c11Entity, procedureX);

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
	PQLEntity pqlC11 = PQLEntity::generateStatement(11);

	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlX = PQLEntity::generateVariable("x");
	PQLEntity pqlY = PQLEntity::generateVariable("y");
	PQLEntity pqlZ = PQLEntity::generateVariable("z");
	PQLEntity pql0 = PQLEntity::generateConstant(0);
	PQLEntity pql1 = PQLEntity::generateConstant(1);
	PQLEntity pql3 = PQLEntity::generateConstant(3);
	PQLEntity pql5 = PQLEntity::generateConstant(5);
	PQLEntity pqlProcX = PQLEntity::generateProcedure("x");
	PQLEntity pqlProcY = PQLEntity::generateProcedure("y");

	PQLRelationship pqlFollowsR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsP4I5 = PQLRelationship(pqlP4, pqlI5);
	PQLRelationship pqlFollowsW3I7 = PQLRelationship(pqlW3, pqlI7);

	PQLRelationship pqlFollowsStarR1A2 = PQLRelationship(pqlR1, pqlA2);
	PQLRelationship pqlFollowsStarR1W3 = PQLRelationship(pqlR1, pqlW3);
	PQLRelationship pqlFollowsStarR1I7 = PQLRelationship(pqlR1, pqlI7);

	PQLRelationship pqlFollowsStarA2W3 = PQLRelationship(pqlA2, pqlW3);
	PQLRelationship pqlFollowsStarA2I7 = PQLRelationship(pqlA2, pqlI7);

	PQLRelationship pqlFollowsStarP4I5 = PQLRelationship(pqlP4, pqlI5);

	PQLRelationship pqlFollowsStarW3I7 = PQLRelationship(pqlW3, pqlI7);

	PQLRelationship pqlParentW3P4 = PQLRelationship(pqlW3, pqlP4);
	PQLRelationship pqlParentW3I5 = PQLRelationship(pqlW3, pqlI5);
	PQLRelationship pqlParentI5A6 = PQLRelationship(pqlI5, pqlA6);
	PQLRelationship pqlParentI7A8 = PQLRelationship(pqlI7, pqlA8);
	PQLRelationship pqlParentI7A9 = PQLRelationship(pqlI7, pqlA9);

	PQLRelationship pqlParentStarW3P4 = PQLRelationship(pqlW3, pqlP4);
	PQLRelationship pqlParentStarW3I5 = PQLRelationship(pqlW3, pqlI5);
	PQLRelationship pqlParentStarW3A6 = PQLRelationship(pqlW3, pqlA6);

	PQLRelationship pqlParentStarI5A6 = PQLRelationship(pqlI5, pqlA6);

	PQLRelationship pqlParentStarI7A8 = PQLRelationship(pqlI7, pqlA8);
	PQLRelationship pqlParentStarI7A9 = PQLRelationship(pqlI7, pqlA9);

	PQLRelationship pqlModifiesSR1Y = PQLRelationship(pqlR1, pqlY);
	PQLRelationship pqlModifiesSA2X = PQLRelationship(pqlA2, pqlX);
	PQLRelationship pqlModifiesSA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlModifiesSA8Y = PQLRelationship(pqlA8, pqlY);
	PQLRelationship pqlModifiesSA9Y = PQLRelationship(pqlA9, pqlY);

	PQLRelationship pqlModifiesSW3X = PQLRelationship(pqlW3, pqlX);
	PQLRelationship pqlModifiesSI5X = PQLRelationship(pqlI5, pqlX);
	PQLRelationship pqlModifiesSI7Y = PQLRelationship(pqlA9, pqlY);

	PQLRelationship pqlModifiesPMainX = PQLRelationship(pqlMain, pqlX);
	PQLRelationship pqlModifiesPMainY = PQLRelationship(pqlMain, pqlY);

	PQLRelationship pqlModifiesPXX = PQLRelationship(pqlProcX, pqlX);
	PQLRelationship pqlModifiesPXY = PQLRelationship(pqlProcX, pqlY);

	PQLRelationship pqlModifiesPYX = PQLRelationship(pqlProcY, pqlX);
	PQLRelationship pqlModifiesPYY = PQLRelationship(pqlProcY, pqlY);

	PQLRelationship pqlUsesSA2Y = PQLRelationship(pqlA2, pqlY);
	PQLRelationship pqlUsesSP4Z = PQLRelationship(pqlP4, pqlZ);
	PQLRelationship pqlUsesSA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlUsesSA9Z = PQLRelationship(pqlA9, pqlZ);

	PQLRelationship pqlUsesSW3X = PQLRelationship(pqlW3, pqlX);
	PQLRelationship pqlUsesSI5Y = PQLRelationship(pqlI5, pqlY);
	PQLRelationship pqlUsesSI5X = PQLRelationship(pqlI5, pqlX);
	PQLRelationship pqlUsesSI7Y = PQLRelationship(pqlI7, pqlY);

	PQLRelationship pqlUsesPMainY = PQLRelationship(pqlMain, pqlY);
	PQLRelationship pqlUsesPMainX = PQLRelationship(pqlMain, pqlX);
	PQLRelationship pqlUsesPMainZ = PQLRelationship(pqlMain, pqlZ);

	PQLRelationship pqlUsesPXY = PQLRelationship(pqlProcX, pqlY);
	PQLRelationship pqlUsesPXX = PQLRelationship(pqlProcX, pqlX);
	PQLRelationship pqlUsesPXZ = PQLRelationship(pqlProcX, pqlZ);

	PQLRelationship pqlUsesPYY = PQLRelationship(pqlProcY, pqlY);
	PQLRelationship pqlUsesPYX = PQLRelationship(pqlProcY, pqlX);
	PQLRelationship pqlUsesPYZ = PQLRelationship(pqlProcY, pqlZ);

	PQLRelationship pqlCallsC10PMain = PQLRelationship(pqlC10, pqlMain);
	PQLRelationship pqlCallsC11PX = PQLRelationship(pqlC11, pqlProcX);

	// Pattern
	Pattern a2Pattern = Pattern::createAssignPattern(2, " x ", " y ");
	Pattern a6Pattern = Pattern::createAssignPattern(6, " x ", " x 1 - ");
	Pattern a8Pattern = Pattern::createAssignPattern(8, " y ", " 3 ");
	Pattern a9Pattern = Pattern::createAssignPattern(9,  " y ", " z ");

	PQLRelationship pqlPatternA2X = PQLRelationship(pqlA2, pqlX);
	PQLRelationship pqlPatternA6X = PQLRelationship(pqlA6, pqlX);
	PQLRelationship pqlPatternA8Y = PQLRelationship(pqlA8, pqlY);
	PQLRelationship pqlPatternA9Y = PQLRelationship(pqlA9, pqlY);

	// Clause Arguments
	ClauseArgument stmtArg = ClauseArgument::createStmtArg("s");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument printArg = ClauseArgument::createPrintArg("p");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument whileArg = ClauseArgument::createWhileArg("w");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument procArg = ClauseArgument::createProcedureArg("p");
	ClauseArgument callArg = ClauseArgument::createCallArg("ca");
	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
	ClauseArgument constArg = ClauseArgument::createConstantArg("c");
	ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();

	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondAssignArg = ClauseArgument::createAssignArg("a2");
	ClauseArgument secondIfArg = ClauseArgument::createIfArg("i1");
	ClauseArgument secondVarArg = ClauseArgument::createVariableArg("v2");

	ClauseArgument lineNumber1Arg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineNumber2Arg = ClauseArgument::createLineNumberArg("2");
	ClauseArgument lineNumber3Arg = ClauseArgument::createLineNumberArg("3");
	ClauseArgument lineNumber4Arg = ClauseArgument::createLineNumberArg("4");
	ClauseArgument lineNumber10Arg = ClauseArgument::createLineNumberArg("10");
	ClauseArgument lineNumber11Arg = ClauseArgument::createLineNumberArg("11");
	ClauseArgument XStringLiteralArg = ClauseArgument::createStringLiteralArg("x");
	ClauseArgument YStringLiteralArg = ClauseArgument::createStringLiteralArg("y");
	ClauseArgument ZStringLiteralArg = ClauseArgument::createStringLiteralArg("z");
	ClauseArgument mainStringLiteralArg = ClauseArgument::createStringLiteralArg("main");
	ClauseArgument nonExistentStringLiteralArg = ClauseArgument::createStringLiteralArg("zzzzz");

	ClauseArgument stmtStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(stmtArg);
	ClauseArgument assignStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(assignArg);
	ClauseArgument callStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(callArg);
	ClauseArgument readStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(readArg);
	ClauseArgument printStmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(printArg);

	ClauseArgument callProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(callArg);
	ClauseArgument readVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(readArg);
	ClauseArgument printVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(printArg);

	ClauseArgument variableVarNameAttributeArg = ClauseArgument::createVarNameAttributeArg(varArg);
	ClauseArgument constantValueAttributeArg = ClauseArgument::createValueAttributeArg(constArg);
	ClauseArgument procedureProcNameAttributeArg = ClauseArgument::createProcNameAttributeArg(procArg);

	ClauseArgument a2PatternStringArg = ClauseArgument::createPatternStringArg("y");
	ClauseArgument a6PatternStringArg = ClauseArgument::createPatternStringArg("x 1 -");
	ClauseArgument a8PatternStringArg = ClauseArgument::createPatternStringArg("3");
	ClauseArgument a9PatternStringArg = ClauseArgument::createPatternStringArg("z");
	ClauseArgument nonExistentPatternStringArg = ClauseArgument::createPatternStringArg("aAaAaA");

	ClauseArgument a2PatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_y_");
	ClauseArgument a6FirstPatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_x 1 -_");
	ClauseArgument a6SecondPatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_x_");
	ClauseArgument a6ThirdPatternStringWithWildcardsArg = ClauseArgument::createPatternStringArg("_1_");

}

TEST_CASE("QueryEvaluator: test evaluate") {
	auto testEvaluate = [](Query query,
						  set<string> expectedSet,
						  shared_ptr<PKB> pkb) {
		// given
		QueryEvaluator evaluator = QueryEvaluator();
		shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

		// when
		set<string> actualSet = evaluator.evaluate(query, pkbInterface);

		// then
		REQUIRE(actualSet == expectedSet);

	};

	// ------ PKB ------
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ readEntity, a2Entity, whileEntity, printEntity, if5Entity, a6Entity, if7Entity, a8Entity, a9Entity,
							 c10Entity, c11Entity, main, x, y, z, zeroEntity, oneEntity, threeEntity, fiveEntity, procedureX,
							 procedureY };

	vector<Relationship> relationships{ followsReadA2, followsA2While, followsPrintIf5, followsWhileIf7, followsStarReadA2,
										followsStarReadWhile, followsStarReadIf7, followsStarA2While, followsStarA2If7,
										followsStarWhileIf7, followsStarPrintIf5,

										parentWhilePrint, parentWhileIf5, parentIf5A6, parentIf7A8, parentIf7A9,
										parentStarWhilePrint, parentStarWhileIf5, parentStarWhileA6, parentStarIf5A6,
										parentStarIf7A8, parentStarIf7A9,

										modifiesSReadY, modifiesSA2X, modifiesSA6X, modifiesSA8Y, modifiesSA9Y,
										modifiesSWhileX, modifiesSIf5X, modifiesSIf7Y, modifiesPMainY, modifiesPMainX,
										modifiesPXY, modifiesPXX, modifiesPYY, modifiesPYX,

										usesSA2Y, usesSPrintZ, usesSA6X, usesSA9Z, usesSWhileX, usesSIf5Y, usesSIf5X,
										usesSIf7Y, usesPMainY, usesPMainX, usesPMainZ, usesPXY, usesPXX, usesPXZ, usesPYY,
										usesPYX, usesPYZ,

										callsC10PMain, callsC11PX};

	vector<Pattern> patterns{ a2Pattern , a6Pattern , a8Pattern , a9Pattern };

	// ------ PKB ------
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);
	pkb->addPatterns(patterns);

	// ------ QPS ------
	list<shared_ptr<RelationshipClause>> emptyRelationships{}; 
	list<shared_ptr<PatternClause>> emptyPatterns{};
	list<shared_ptr<WithClause>> emptyWiths{};
	shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ stmtArg }));
	Query query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
	set<string> expectedSet = set<string>{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"};

	SECTION("Select single return value only") {
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({readArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({printArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({whileArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ifArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({procArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"main", "x", "y"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({constArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"0", "1", "3", "5"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select multiple return values only") {
		// Select <p, w>
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({printArg, whileArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"4 3"};
		testEvaluate(query, expectedSet, pkb);

		// Select <a, v>
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg, varArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2 x", "2 y", "2 z", "6 x", "6 y", "6 z", "8 x", "8 y", "8 z", "9 x", "9 y", "9 z"};
		testEvaluate(query, expectedSet, pkb);

		// Select <v, v>
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg, varArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x x", "y y", "z z"};
		testEvaluate(query, expectedSet, pkb);

		// Select <v, r, v>
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg, readArg, varArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x 1 x", "y 1 y", "z 1 z"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select boolean return type only") {
		// Select BOOLEAN
		selectClause = make_shared<SelectClause>(SelectClause::createBooleanSelectClause());
		query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"TRUE"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Follows") {

		// Select s1 such that Follows(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1", "2", "3", "4"};
		testEvaluate(query, expectedSet, pkb);

		// Select s2 such that Follows(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({secondStmtArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2", "3", "5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s1 such that Follows*(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1", "2", "3", "4"};
		testEvaluate(query, expectedSet, pkb);

		// Select s2 such that Follows*(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({secondStmtArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2", "3", "5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows(r, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(readArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows*(r, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(readArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2", "3", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows(a, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(assignArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows*(a, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(assignArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows(w, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows*(w, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows(i, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Follows*(i, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Parent") {

		// Select s1 such that Parent(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ParentClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3", "5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s2 such that Parent(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({secondStmtArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"4", "5", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select i such that Parent(i, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ifArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ParentClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Parent(i, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select w such that Parent(w, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({whileArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ParentClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Parent(w, s)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"4", "5"};
		testEvaluate(query, expectedSet, pkb);

		// Select s such that Parent*(w, s)
		relationshipClause = shared_ptr<RelationshipClause>(new ParentTClause(whileArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"4", "5", "6"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Modifies") {

		// Select s such that Modifies(s, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(stmtArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1", "2", "3", "5", "6", "7", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(s, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select r such that Modifies(r, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({readArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(readArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(r, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"y"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(a, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select w such that Modifies(w, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({whileArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(whileArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(w, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x"};
		testEvaluate(query, expectedSet, pkb);

		// Select i such that Modifies(i, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ifArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(ifArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(i, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select pr such that Modifies(pr, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({procArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesPClause(procArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"main", "x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(pr, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

	}

	SECTION("Select + Uses") {

		// Select s such that Uses(s, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(stmtArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2", "3", "4", "5", "6", "7", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(s, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

		// Select p such that Uses(p, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({printArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(printArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(p, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"z"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Uses(a, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(assignArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2", "6", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(a, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

		// Select w such that Uses(w, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({whileArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(whileArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(w, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x"};
		testEvaluate(query, expectedSet, pkb);

		// Select i such that Uses(i, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ifArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(ifArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"5", "7"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(i, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select pr such that Uses(pr, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({procArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new UsesPClause(procArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"main", "x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Uses(pr, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

	}

	SECTION("Select + Assign Pattern") {
		// Select a pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(_, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, wildcardArg, wildcardArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a("x", _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, XStringLiteralArg, wildcardArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2", "6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a("y", _)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, YStringLiteralArg, wildcardArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a("z", _)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, nonExistentStringLiteralArg, wildcardArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "y")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a2PatternStringArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "x - 1")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6PatternStringArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "3")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a8PatternStringArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"8"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "z")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a9PatternStringArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, "aAaAaA")
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, nonExistentPatternStringArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"y"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a2PatternStringWithWildcardsArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"x1-"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6FirstPatternStringWithWildcardsArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"x"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6SecondPatternStringWithWildcardsArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select a pattern a(v, _"1"_)
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6ThirdPatternStringWithWildcardsArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + WithBothExact") {
		// Select v with "x" = "x"
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		shared_ptr<WithClause> withClause = shared_ptr<WithClause>(new WithBothExactClause(XStringLiteralArg, XStringLiteralArg));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"x", "y", "z"};
		testEvaluate(query, expectedSet, pkb);

		// Select v with "x" = "y"
		withClause = shared_ptr<WithClause>(new WithBothExactClause(XStringLiteralArg, YStringLiteralArg));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select c with 1 = 1
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({constArg}));
		withClause = shared_ptr<WithClause>(new WithBothExactClause(lineNumber1Arg, lineNumber1Arg));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"0", "1", "3", "5"};
		testEvaluate(query, expectedSet, pkb);

		// Select c with 1 = 3
		withClause = shared_ptr<WithClause>(new WithBothExactClause(lineNumber1Arg, lineNumber3Arg));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + WithOneExact") {
		/* With only default attributes */

		// Select c with c.value = 1;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({constArg}));
		shared_ptr<WithClause> withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber1Arg, {constArg, constantValueAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		// Select pr with pr.procName = "main"
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({procArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(mainStringLiteralArg, {procArg, procedureProcNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"main"};
		testEvaluate(query, expectedSet, pkb);

		// Select v with v.varName = "x"
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(XStringLiteralArg, {varArg, variableVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"x"};
		testEvaluate(query, expectedSet, pkb);

		// Select s with s.stmt# = 3;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber3Arg, {stmtArg, stmtStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"3"};
		testEvaluate(query, expectedSet, pkb);

		// Select a with a.stmt# = 2;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber2Arg, {assignArg, assignStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);

		/* With default/non-default attributes */

		// Select r with r.stmt# = 1;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({readArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber1Arg, {readArg, readStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		// Select r with r.varName = "y";
		withClause = shared_ptr<WithClause>(new WithOneExactClause(YStringLiteralArg, {readArg, readVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		// Select r.varName with r.stmt# = 1;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({readArg, readVarNameAttributeArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber1Arg, {readArg, readStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"y"};
		testEvaluate(query, expectedSet, pkb);

		// Select r.varName with r.varName = "y";
		withClause = shared_ptr<WithClause>(new WithOneExactClause(YStringLiteralArg, {readArg, readVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"y"};
		testEvaluate(query, expectedSet, pkb);

		// Select ca with ca.stmt# = 10;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber10Arg, {callArg, callStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"10"};
		testEvaluate(query, expectedSet, pkb);

		// Select ca with ca.procName = "main";
		withClause = shared_ptr<WithClause>(new WithOneExactClause(mainStringLiteralArg, {callArg, callProcNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"10"};
		testEvaluate(query, expectedSet, pkb);

		// Select ca.procName with ca.stmt# = 10;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg, callProcNameAttributeArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber10Arg, {callArg, callStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"main"};
		testEvaluate(query, expectedSet, pkb);

		// Select ca with ca.procName = "main";
		withClause = shared_ptr<WithClause>(new WithOneExactClause(mainStringLiteralArg, {callArg, callProcNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"main"};
		testEvaluate(query, expectedSet, pkb);

		// Select p with p.stmt# = 4;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({printArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber4Arg, {printArg, printStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		// Select p with p.varName = "z";
		withClause = shared_ptr<WithClause>(new WithOneExactClause(ZStringLiteralArg, {printArg, printVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		// Select p.varName with p.stmt# = 4;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({printArg, printVarNameAttributeArg}));
		withClause = shared_ptr<WithClause>(new WithOneExactClause(lineNumber4Arg, {printArg, printStmtNumAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"z"};
		testEvaluate(query, expectedSet, pkb);

		withClause = shared_ptr<WithClause>(new WithOneExactClause(ZStringLiteralArg, {printArg, printVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"z"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + WithNoExact") {
		// Select ca with ca.procName = v.varName;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg}));
		shared_ptr<WithClause> withClause = shared_ptr<WithClause>(new WithNoExactClause({callArg, callProcNameAttributeArg}, {varArg, variableVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"11"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select multiple return values + With") {
		// Select <ca, v> with ca.procName = v.varName;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg, varArg}));
		shared_ptr<WithClause> withClause = shared_ptr<WithClause>(new WithNoExactClause({callArg, callProcNameAttributeArg}, {varArg, variableVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"11 x"};
		testEvaluate(query, expectedSet, pkb);

		// Select <ca.procName, v> with ca.procName = v.varName;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg, callProcNameAttributeArg, varArg}));
		withClause = shared_ptr<WithClause>(new WithNoExactClause({callArg, callProcNameAttributeArg}, {varArg, variableVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"x x"};
		testEvaluate(query, expectedSet, pkb);

		// Select <ca.procName, r.varName, r> with ca.procName = v.varName;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg, callProcNameAttributeArg, readArg, readVarNameAttributeArg, readArg}));
		withClause = shared_ptr<WithClause>(new WithNoExactClause({callArg, callProcNameAttributeArg}, {varArg, variableVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"x y 1"};
		testEvaluate(query, expectedSet, pkb);

		// Select <ca.procName, c, ca.procName, v> with ca.procName = v.varName;
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg, callProcNameAttributeArg, callArg, callArg, callProcNameAttributeArg, varArg}));
		withClause = shared_ptr<WithClause>(new WithNoExactClause({callArg, callProcNameAttributeArg}, {varArg, variableVarNameAttributeArg}));
		query = Query(selectClause, emptyRelationships, emptyPatterns, list<shared_ptr<WithClause>>{withClause});
		expectedSet = set<string>{"x 11 x x"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - non-empty results, two matching clauses") {
		// Select a such that Modifies(a, v) pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(a, v) pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, v) pattern a(v, _"x"_)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6SecondPatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"6"};
		testEvaluate(query, expectedSet, pkb);

		// Select v such that Modifies(a, v) pattern a(v, _"x"_)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a6SecondPatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"x"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Follows(_, a) pattern a(_, "y") (Double wildcard, exposed bug in combinedTableJoin, now fixed)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(wildcardArg, assignArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, wildcardArg, a2PatternStringWithWildcardsArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2"};
		testEvaluate(query, expectedSet, pkb);
	}


	SECTION("Select + Such That + Pattern - non-empty results, one matching clause") {
		// Select a such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
		ClauseArgument stringLiteralArg = ClauseArgument::createStringLiteralArg("y");
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(lineNumArg, stringLiteralArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2", "6", "8", "9"};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(1, "y") pattern a("x", _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, XStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2", "6"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - non-empty results, no matching clause") {
		// Select r such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({readArg}));
		ClauseArgument lineNumArg = ClauseArgument::createLineNumberArg("1");
		ClauseArgument stringLiteralArg = ClauseArgument::createStringLiteralArg("y");
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(lineNumArg, stringLiteralArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"1"};
		testEvaluate(query, expectedSet, pkb);

		// Select p such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({printArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"4"};
		testEvaluate(query, expectedSet, pkb);

		// Select pr such that Modifies(1, "y") pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({procArg}));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"main", "x", "y"};
		testEvaluate(query, expectedSet, pkb);

		// Select c such that Modifies(a, v) pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({callArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"10", "11"};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - empty results, no tables empty") {
		// Select a such that Modifies(a, "y") pattern a("x", _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, YStringLiteralArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, XStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, "x") pattern a("y", _)
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, XStringLiteralArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, YStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, "x") pattern a(v, "z")
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, XStringLiteralArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a9PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Follows*(a, s) pattern a(v, _"3"_)
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(assignArg, stmtArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, a8PatternStringArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - empty results, one relationship table empty") {
		// Select a such that Follows*(i, a) pattern a(v, _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(ifArg, assignArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select a such that Modifies(a, v) pattern a("z", _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, nonExistentStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select + Such That + Pattern - empty results, two relationship tables empty") {
		// Select a such that Follows*(i, a) pattern a("z", _)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(ifArg, assignArg));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, nonExistentStringLiteralArg, wildcardArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Multi-clause: 4 non-empty groups") {
		/* Group 1: Select v such that Follows(r, a) such that Modifies(a, v) pattern a(_, "y") with r.varName = "y"
		*  Group 2: such that Parent*(w, a2) pattern a2("x", _"x"_)
		*  Group 3: such that Uses(s2, _) such that Follows (3, s2)
		*  Group 4: with "x" = "x" with 1 = 1
		*/
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
		
		shared_ptr<RelationshipClause> group1Follows = shared_ptr<RelationshipClause>(new FollowsClause(readArg, assignArg));
		shared_ptr<RelationshipClause> group1ModifiesS = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		shared_ptr<PatternClause> group1PatternAssign = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, wildcardArg, a2PatternStringArg));
		shared_ptr<WithClause> group1WithOneExact = shared_ptr<WithClause>(new WithOneExactClause(YStringLiteralArg, {readArg, readVarNameAttributeArg}));
		
		shared_ptr<RelationshipClause> group2ParentT = shared_ptr<RelationshipClause>(new ParentTClause(whileArg, secondAssignArg));
		shared_ptr<PatternClause> group2PatternAssign = shared_ptr<PatternClause>(new PatternAssignClause(secondAssignArg, XStringLiteralArg, a6SecondPatternStringWithWildcardsArg));

		shared_ptr<RelationshipClause> group3UsesS = shared_ptr<RelationshipClause>(new UsesSClause(secondStmtArg, wildcardArg));
		shared_ptr<RelationshipClause> group3Follows = shared_ptr<RelationshipClause>(new FollowsClause(lineNumber3Arg, secondStmtArg));

		shared_ptr<WithClause> group4WithBothExactStringLiteral = shared_ptr<WithClause>(new WithBothExactClause(XStringLiteralArg, XStringLiteralArg));
		shared_ptr<WithClause> group4WithBothExactLineNum = shared_ptr<WithClause>(new WithBothExactClause(lineNumber1Arg, lineNumber1Arg));

		list<shared_ptr<RelationshipClause>> suchThatClauses = list<shared_ptr<RelationshipClause>>{
			group1Follows, group1ModifiesS, group2ParentT, group3UsesS, group3Follows,
		};
		list<shared_ptr<PatternClause>> patternClauses = list<shared_ptr<PatternClause>>{ group1PatternAssign, group2PatternAssign };

		list<shared_ptr<WithClause>> withClauses = list<shared_ptr<WithClause>>{ group1WithOneExact, group4WithBothExactStringLiteral, group4WithBothExactLineNum };

		query = Query(selectClause, suchThatClauses, patternClauses, withClauses);
		expectedSet = set<string>{ "x" };
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Multi-clause: 4 non-empty groups, 1 empty group") {
		/* Group 1: Select v such that Follows(s, a) such that Modifies(a, v) pattern a(_, "y") with r.varName = "y"
		*  Group 2: such that Parent*(w, a2) pattern a2("x", _"x"_)
		*  Group 3: such that Uses(s2, _) such that Follows (3, s2)
		*  Group 4: such that Parent(i, i)
		*/
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));

		shared_ptr<RelationshipClause> group1Follows = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, assignArg));
		shared_ptr<RelationshipClause> group1ModifiesS = shared_ptr<RelationshipClause>(new ModifiesSClause(assignArg, varArg));
		shared_ptr<PatternClause> group1PatternAssign = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, wildcardArg, a2PatternStringArg));
		shared_ptr<WithClause> group1WithOneExact = shared_ptr<WithClause>(new WithOneExactClause(YStringLiteralArg, {readArg, readVarNameAttributeArg}));

		shared_ptr<RelationshipClause> group2ParentT = shared_ptr<RelationshipClause>(new ParentTClause(whileArg, secondAssignArg));
		shared_ptr<PatternClause> group2PatternAssign = shared_ptr<PatternClause>(new PatternAssignClause(secondAssignArg, XStringLiteralArg, a6SecondPatternStringWithWildcardsArg));

		shared_ptr<RelationshipClause> group3UsesS = shared_ptr<RelationshipClause>(new UsesSClause(secondStmtArg, wildcardArg));
		shared_ptr<RelationshipClause> group3Follows = shared_ptr<RelationshipClause>(new FollowsClause(lineNumber3Arg, secondStmtArg));

		shared_ptr<RelationshipClause> group4Parent = shared_ptr<RelationshipClause>(new ParentClause(ifArg, ifArg));

		list<shared_ptr<RelationshipClause>> suchThatClauses = list<shared_ptr<RelationshipClause>>{
			group1Follows, group1ModifiesS, group2ParentT, group3UsesS, group3Follows, group4Parent,
		};
		list<shared_ptr<PatternClause>> patternClauses = list<shared_ptr<PatternClause>>{ group1PatternAssign, group2PatternAssign };
		
		list<shared_ptr<WithClause>> withClauses = list<shared_ptr<WithClause>>{ group1WithOneExact };
		
		query = Query(selectClause, suchThatClauses, patternClauses, withClauses);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		/* Group 1: Select v such that Follows(s, a) such that Modifies(a, v) pattern a(_, "y") with r.varName = "y"
		*  Group 2: such that Parent*(w, a2) pattern a2("x", _"x"_)
		*  Group 3: such that Uses(s2, _) such that Follows (3, s2)
		*  Group 4: with "x" = "y"
		*/
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));

		shared_ptr<WithClause> group4WithBothExactInvalidStringLiteral = shared_ptr<WithClause>(new WithBothExactClause(XStringLiteralArg, YStringLiteralArg));

		suchThatClauses = list<shared_ptr<RelationshipClause>>{
				group1Follows, group1ModifiesS, group2ParentT, group3UsesS, group3Follows,
		};

		withClauses = list<shared_ptr<WithClause>>{ group1WithOneExact, group4WithBothExactInvalidStringLiteral };

		query = Query(selectClause, suchThatClauses, patternClauses, withClauses);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select multiple return values + constraints") {

		// Select <s1, s2> such that Follows(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg, secondStmtArg}));
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1 2", "2 3", "3 7", "4 5"};
		testEvaluate(query, expectedSet, pkb);

		// Select <s2, s1> such that Follows*(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({secondStmtArg, stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2 1", "3 2", "3 1", "5 4", "7 3", "7 2", "7 1"};
		testEvaluate(query, expectedSet, pkb);

		// Select <s1, s2> such that Parent(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg, secondStmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ParentClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"3 4", "3 5", "5 6", "7 8", "7 9"};
		testEvaluate(query, expectedSet, pkb);

		// Select <s2, s1> such that Parent*(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({secondStmtArg, stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ParentTClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"4 3", "5 3", "6 3", "6 5", "8 7", "9 7"};
		testEvaluate(query, expectedSet, pkb);

		// Select <s, v> such that Modifies(s, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg, varArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesSClause(stmtArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1 y", "2 x", "3 x", "5 x", "6 x", "7 y", "8 y", "9 y"};
		testEvaluate(query, expectedSet, pkb);

		// Select <pr, v> such that Modifies(pr, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({procArg, varArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new ModifiesPClause(procArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"main x", "main y", "x x", "x y", "y x", "y y"};
		testEvaluate(query, expectedSet, pkb);

		// Select <s, v> such that Uses(s, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg, varArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new UsesSClause(stmtArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"2 y", "3 x", "4 z", "5 x", "5 y", "6 x", "7 y", "9 z"};
		testEvaluate(query, expectedSet, pkb);

		// Select <pr, v> such that Uses(pr, v)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({procArg, varArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new UsesPClause(procArg, varArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"main x", "main y", "main z", "x x", "x y", "x z", "y x", "y y", "y z"};
		testEvaluate(query, expectedSet, pkb);

		// Select <a, v> pattern a(v,_)
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({assignArg, varArg}));
		shared_ptr<PatternClause> patternClause = shared_ptr<PatternClause>(new PatternAssignClause(assignArg, varArg, wildcardArg));
		query = Query(selectClause, emptyRelationships, list<shared_ptr<PatternClause>>{patternClause}, emptyWiths);
		expectedSet = set<string>{"2 x", "6 x", "8 y", "9 y"};
		testEvaluate(query, expectedSet, pkb);

		// Select <s1, s1> such that Follows(s1, s2) - repeated select args
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg, stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1 1", "2 2", "3 3", "4 4"};
		testEvaluate(query, expectedSet, pkb);

		// Select <s1, i> such that Follows(s1, s2) - one select arg not in combined table
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({stmtArg, ifArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"1 5", "1 7", "2 5", "2 7", "3 5", "3 7", "4 5", "4 7"};
		testEvaluate(query, expectedSet, pkb);

		// Select <v, i> such that Follows(s1, s2) - all select args not in combined table
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg, ifArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"x 5", "x 7", "y 5", "y 7", "z 5", "z 7"};
		testEvaluate(query, expectedSet, pkb);

		// Select <i, s> such that Follows(i, s) - no matches
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ifArg, stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);

		// Select <i, s> such that Follows*(i, s) - no matches
		selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ifArg, stmtArg}));
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{};
		testEvaluate(query, expectedSet, pkb);
	}

	SECTION("Select boolean return type + constraints") {

		// Select BOOLEAN such that Follows(s1, s2)
		selectClause = make_shared<SelectClause>(SelectClause::createBooleanSelectClause());
		shared_ptr<RelationshipClause> relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"TRUE"};
		testEvaluate(query, expectedSet, pkb);

		// Select BOOLEAN such that Follows*(s1, s2)
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(stmtArg, secondStmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"TRUE"};
		testEvaluate(query, expectedSet, pkb);

		// Select BOOLEAN such that Follows(i, s)
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"FALSE"};
		testEvaluate(query, expectedSet, pkb);

		// Select BOOLEAN such that Follows*(i, s)
		relationshipClause = shared_ptr<RelationshipClause>(new FollowsTClause(ifArg, stmtArg));
		query = Query(selectClause, list<shared_ptr<RelationshipClause>>{relationshipClause}, emptyPatterns, emptyWiths);
		expectedSet = set<string>{"FALSE"};
		testEvaluate(query, expectedSet, pkb);
	}
}


