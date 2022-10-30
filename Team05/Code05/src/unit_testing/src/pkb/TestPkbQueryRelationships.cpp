#include "catch.hpp"

#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextRelationship.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <pkb/PkbException.h>
#include <pkb/PKB.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbGraphNode.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/SourceProcessor.h>

#include <algorithm>
#include <memory>
#include <string>
using namespace std;


TEST_CASE("Test add and retrieve table relationships by type and lhs rhs") {
	auto test = [](PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, vector<PQLRelationship> expectedRelationships, vector<Relationship> toAdd) {
		// given
		PKB pkb;

		// when 
		pkb.addRelationships(toAdd);

		// then 
		vector<PQLRelationship> all = pkb.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs);
		REQUIRE(expectedRelationships.size() == all.size());
		for (int i = 0; i < expectedRelationships.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedRelationships[i]) != all.end());
		}
	};

	SECTION("Follows") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createReadEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createAssignEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = {
			Relationship::createFollowsRelationship(statement1, statement2),
			Relationship::createFollowsRelationship(statement2, statement3),
			Relationship::createFollowsRelationship(statement3,statement4),

		};

		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one a specific line
		lhs = ClauseArgument::createLineNumberArg("1");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);

		// test 4: both different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::FOLLOWS, lhs, rhs, expectedRelationships, toAdd);


	};
	SECTION("FollowsStar") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createReadEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createAssignEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = {
			Relationship::createFollowsTRelationship(statement1, statement2),
			Relationship::createFollowsTRelationship(statement2, statement3),
			Relationship::createFollowsTRelationship(statement3, statement4),
			Relationship::createFollowsTRelationship(statement1, statement3),
			Relationship::createFollowsTRelationship(statement1, statement4),
			Relationship::createFollowsTRelationship(statement2, statement4)
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult2, statementResult4),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult3, statementResult4),

		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one a specific line
		lhs = ClauseArgument::createLineNumberArg("1");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: both different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs, expectedRelationships, toAdd);

	};
	SECTION("Parent") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createIfEntity(1);
		Entity statement2 = Entity::createAssignEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = {
			Relationship::createParentRelationship(statement1, statement2),
			Relationship::createParentRelationship(statement3, statement4),
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 2: one wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 3: both statement synonyms, but different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one line number
		lhs = ClauseArgument::createLineNumberArg("3");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENT, lhs, rhs, expectedRelationships, toAdd);
	};
	SECTION("ParentStar") {
		// shared, as if 4 statements in a block
		Entity statement1 = Entity::createIfEntity(1);
		Entity statement2 = Entity::createAssignEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createCallEntity(4);
		vector<Relationship> toAdd = {
			Relationship::createParentTRelationship(statement1, statement2),
			Relationship::createParentTRelationship(statement3, statement4),
			Relationship::createParentTRelationship(statement1, statement4),
			Relationship::createParentTRelationship(statement1, statement3),
		};
		// shared, as PQLEntities
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: both statement synonyms
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult3),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 2: one wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s2");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult3),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 3: both statement synonyms, but different types
		lhs = ClauseArgument::createAssignArg("a");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one line number
		lhs = ClauseArgument::createLineNumberArg("3");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			PQLRelationship(statementResult3, statementResult4),
		};
		test(PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs, expectedRelationships, toAdd);

	};
	SECTION("Uses") {
		/*
			procedure p {
			1.	x = x + 1;
			2.	print x;
			3.	if (x == 1) then {
			4. 		call x;
				}
			}
		*/
		Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("p"));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("x"));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createPrintEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createCallEntity(4);

		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createUsesRelationship(statement1, x),
			Relationship::createUsesRelationship(statement2, x),
			Relationship::createUsesRelationship(statement3, x),
			Relationship::createUsesRelationship(procedure, x),
			Relationship::createUsesRelationship(statement4, x),
		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);

		// test 1: assign and variable
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 2: print and variable
		lhs = ClauseArgument::createPrintArg("pn");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult2, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 3a: if and variable
		lhs = ClauseArgument::createIfArg("ifs");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult3, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 3b: while and variable
		lhs = ClauseArgument::createWhileArg("w");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {

		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 4: procedure and variable
		lhs = ClauseArgument::createProcedureArg("p");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 5: call and variable
		lhs = ClauseArgument::createCallArg("c");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult4, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

		// test 6: rhs wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
			PQLRelationship(statementResult2, xResult),
			PQLRelationship(statementResult3, xResult),
			PQLRelationship(statementResult4, xResult),
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::USES, lhs, rhs, expectedRelationships, toAdd);

	};

	SECTION("Modifies") {
		/*
			procedure p {
			1.	x = 1;
			2.	read x;
			3.	if (x == 1) then {
			4.		x = 2;
			5.		call p; // recursive call
				}

			}
		*/
		Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("p"));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("x"));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createAssignEntity(4);
		Entity statement5 = Entity::createCallEntity(5);
		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createModifiesRelationship(statement1, x),
			Relationship::createModifiesRelationship(statement2, x),
			Relationship::createModifiesRelationship(statement3, x),
			Relationship::createModifiesRelationship(procedure, x),
			Relationship::createModifiesRelationship(statement5, x),
		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);
		PQLEntity statementResult5 = PQLEntity::generateStatement(5);

		// test 1: assign and variable
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createVariableArg("v");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 2: print and variable
		lhs = ClauseArgument::createReadArg("r");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult2, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);


		// test 3a: if and variable
		lhs = ClauseArgument::createIfArg("ifs");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult3, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 3b: while and variable
		lhs = ClauseArgument::createWhileArg("w");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {

		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 4: procedure and variable
		lhs = ClauseArgument::createProcedureArg("p");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 5: call and variable
		lhs = ClauseArgument::createCallArg("c");
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult5, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);

		// test 6: rhs wildcard
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createVariableArg("v");
		expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
			PQLRelationship(statementResult2, xResult),
			PQLRelationship(statementResult3, xResult),
			PQLRelationship(statementResult5, xResult),
			PQLRelationship(procedureResult, xResult),
		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);
	};

	SECTION("Calls") {
		/*
			procedure main {
				call alpha;
				call beta;
			}

			procedure alpha {
				call beta;
			}

			procedure beta {}
		*/
		Entity mainEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
		Entity alphaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("alpha"));
		Entity betaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("beta"));
		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createCallsRelationship(mainEntity, alphaEntity),
			Relationship::createCallsRelationship(mainEntity, betaEntity),
			Relationship::createCallsRelationship(alphaEntity, betaEntity),
		};
		// shared, as PQLEntities
		PQLEntity mainProcedureResult = PQLEntity::generateProcedure("main");
		PQLEntity alphaProcedureResult = PQLEntity::generateProcedure("alpha");
		PQLEntity betaProcedureResult = PQLEntity::generateProcedure("beta");


		// test 1: both procedure synonyms
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p1");
		ClauseArgument rhs = ClauseArgument::createProcedureArg("p2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard, one specific procedure
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStringLiteralArg("alpha");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one wildcard, one procedure synonym
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one specific procedure
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);

		// test 5: both specific procedures
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createStringLiteralArg("alpha");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLS, lhs, rhs, expectedRelationships, toAdd);
	};

	SECTION("CallsStar") {
		/*
			procedure main {
				call alpha;
			}

			procedure alpha {
				call beta;
			}

			procedure beta {}
		*/
		Entity mainEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
		Entity alphaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("alpha"));
		Entity betaEntity = Entity::createProcedureEntity(Token::createNameOrKeywordToken("beta"));
		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createCallsTRelationship(mainEntity, alphaEntity),
			Relationship::createCallsTRelationship(mainEntity, betaEntity),
			Relationship::createCallsTRelationship(alphaEntity, betaEntity),

		};
		// shared, as PQLEntities
		PQLEntity mainProcedureResult = PQLEntity::generateProcedure("main");
		PQLEntity alphaProcedureResult = PQLEntity::generateProcedure("alpha");
		PQLEntity betaProcedureResult = PQLEntity::generateProcedure("beta");


		// test 1: both procedure synonyms
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p1");
		ClauseArgument rhs = ClauseArgument::createProcedureArg("p2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);


		// test 2: one wildcard, one specific procedure
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStringLiteralArg("beta");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 3: one wildcard, one procedure synonym
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
			PQLRelationship(alphaProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 4: one specific procedure
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createProcedureArg("p");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
			PQLRelationship(mainProcedureResult, betaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

		// test 5: both specific procedures
		lhs = ClauseArgument::createStringLiteralArg("main");
		rhs = ClauseArgument::createStringLiteralArg("alpha");
		expectedRelationships = {
			PQLRelationship(mainProcedureResult, alphaProcedureResult),
		};
		test(PKBTrackedRelationshipType::CALLSSTAR, lhs, rhs, expectedRelationships, toAdd);

	};

	SECTION("Exact queries (by statement no., by variable name)") {
		/*
			procedure p {
			1.	x = 1;
			2.	read x;
			3.	if (x == 1) then {
			4.		x = 2;
			5.		call p; // recursive call
				}

			}
		*/
		Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("p"));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("x"));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createAssignEntity(4);
		Entity statement5 = Entity::createCallEntity(5);
		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createModifiesRelationship(statement1, x),
			Relationship::createModifiesRelationship(statement2, x),
			Relationship::createModifiesRelationship(statement3, x),
			Relationship::createModifiesRelationship(procedure, x),

		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);
		PQLEntity statementResult5 = PQLEntity::generateStatement(5);

		// test 1: assign and variable
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createStringLiteralArg("x");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, xResult),
		};

		// test 2: none, but returns empty
		lhs = ClauseArgument::createLineNumberArg("4");
		rhs = ClauseArgument::createStringLiteralArg("x");
		expectedRelationships = {

		};
		test(PKBTrackedRelationshipType::MODIFIES, lhs, rhs, expectedRelationships, toAdd);
	};

	SECTION("Next") {
		/*
			procedure p {
			1.	x = 1;
			2.	read x;
			3.	if (x == 1) then {
			4.		x = 2;
			5.		call p; // recursive call
				} else {
			6.		x = 3;
				}
			7.	while (x == 3) {
			8.		x = 2;
				}

			}
		*/
		// entity
		Entity procedure = Entity::createProcedureEntity(Token::createNameOrKeywordToken("p"));
		Entity x = Entity::createVariableEntity(INVALID_LINE_NUMBER, Token::createNameOrKeywordToken("x"));
		Entity statement1 = Entity::createAssignEntity(1);
		Entity statement2 = Entity::createReadEntity(2);
		Entity statement3 = Entity::createIfEntity(3);
		Entity statement4 = Entity::createAssignEntity(4);
		Entity statement5 = Entity::createCallEntity(5);
		Entity statement6 = Entity::createAssignEntity(6);
		Entity statement7 = Entity::createWhileEntity(7);
		Entity statement8 = Entity::createAssignEntity(8);

		// vector of relationships to add
		vector<Relationship> toAdd = {
			Relationship::createNextRelationship(statement1, statement2),
			Relationship::createNextRelationship(statement2, statement3),
			Relationship::createNextRelationship(statement3, statement4), // if to inside if
			Relationship::createNextRelationship(statement4, statement5),
			Relationship::createNextRelationship(statement3, statement6), // if to else
			Relationship::createNextRelationship(statement6, statement7), // else to after
			Relationship::createNextRelationship(statement5, statement7), // if to after
			Relationship::createNextRelationship(statement7, statement8), // whlile
			Relationship::createNextRelationship(statement8, statement7), // while backedge
		};
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");
		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);
		PQLEntity statementResult5 = PQLEntity::generateStatement(5);
		PQLEntity statementResult6 = PQLEntity::generateStatement(6);
		PQLEntity statementResult7 = PQLEntity::generateStatement(7);
		PQLEntity statementResult8 = PQLEntity::generateStatement(8);

		// test 1a: both exact, both inside -> one result
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);

		// test 1b: both exact, one not inside -> no result
		lhs = ClauseArgument::createLineNumberArg("1");
		rhs = ClauseArgument::createLineNumberArg("5");
		expectedRelationships = {
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);

		// test 2: both wildcard
		lhs = ClauseArgument::createWildcardArg(); // is wildcard
		rhs = ClauseArgument::createStmtArg("s"); // essentially wildcard
		expectedRelationships = { // expect all relationships
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult4, statementResult5),
			PQLRelationship(statementResult3, statementResult6),
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult7, statementResult8),
			PQLRelationship(statementResult8, statementResult7),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);



		// test 3: exact, wildcard
		lhs = ClauseArgument::createLineNumberArg("3"); // start at 3
		rhs = ClauseArgument::createWildcardArg();
		expectedRelationships = { // expect all relationships
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult3, statementResult6),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);


		// test 4: wildcard, exact
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createLineNumberArg("7");
		expectedRelationships = { // those that end at 7
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult8, statementResult7),
		};
		test(PKBTrackedRelationshipType::NEXT, lhs, rhs, expectedRelationships, toAdd);



	}
}


TEST_CASE("Test retrieve relationship short circuits to empty result") {
	auto test = [](vector<Relationship> relationshipsToAdd, PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
		// given
		PKB pkb;

		// when 
		pkb.addRelationships(relationshipsToAdd);
		vector<PQLRelationship> retrieved = pkb.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs);

		// then 
		REQUIRE(retrieved.size() == 0);
	};

	// shared, as if 4 statements in a block
	/*
		if ...
			assign ...
			if ...
				call ...
				call ...
	*/
	Entity statement1 = Entity::createIfEntity(1);
	Entity statement2 = Entity::createAssignEntity(2);
	Entity statement3 = Entity::createIfEntity(3);
	Entity statement4 = Entity::createCallEntity(4);
	Entity statement5 = Entity::createCallEntity(5);

	vector<Relationship> toAdd = {
		Relationship::createParentTRelationship(statement1, statement2),
		Relationship::createParentTRelationship(statement3, statement4),
		Relationship::createParentTRelationship(statement1, statement4),
		Relationship::createParentTRelationship(statement1, statement3),
		Relationship::createParentRelationship(statement1, statement2),
		Relationship::createParentRelationship(statement3, statement4),
		Relationship::createFollowsRelationship(statement4, statement5),
		Relationship::createFollowsTRelationship(statement4, statement5),
	};

	SECTION("Follows(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(a,a)") {
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createAssignArg("a");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(1,1)") {
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("1");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(if,if)") {
		ClauseArgument lhs = ClauseArgument::createIfArg("if");
		ClauseArgument rhs = ClauseArgument::createIfArg("if");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}


	SECTION("Follows(r,r)") {
		ClauseArgument lhs = ClauseArgument::createReadArg("r");
		ClauseArgument rhs = ClauseArgument::createReadArg("r");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}


	SECTION("Follows(pr,pr)") {
		ClauseArgument lhs = ClauseArgument::createPrintArg("pr");
		ClauseArgument rhs = ClauseArgument::createPrintArg("pr");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}

	SECTION("Follows(w,w)") {
		ClauseArgument lhs = ClauseArgument::createWhileArg("w");
		ClauseArgument rhs = ClauseArgument::createWhileArg("w");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWS, lhs, rhs);
	}


	SECTION("Follows*(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::FOLLOWSSTAR, lhs, rhs);
	}

	SECTION("Parent(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::PARENT, lhs, rhs);
	}

	SECTION("Parent*(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::PARENTSTAR, lhs, rhs);
	}


	SECTION("Next(s,s)") {
		ClauseArgument lhs = ClauseArgument::createStmtArg("s");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s");
		test(toAdd, PKBTrackedRelationshipType::NEXT, lhs, rhs);
	}
	SECTION("Next(a,a)") {
		ClauseArgument lhs = ClauseArgument::createAssignArg("a");
		ClauseArgument rhs = ClauseArgument::createAssignArg("a");
		test(toAdd, PKBTrackedRelationshipType::NEXT, lhs, rhs);
	}

	SECTION("Next(1,1)") {
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("1");
		test(toAdd, PKBTrackedRelationshipType::NEXT, lhs, rhs);
	}

}


TEST_CASE("Add and get graph relationships by type and lhs and rhs") {

	auto test = [](PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, vector<PQLRelationship> expectedRelationships, vector<shared_ptr<CFGNode>> graphsToAdd) {
		// given pkb 
		PKB pkb;

		// when add and retrieve relationships 
		pkb.addCfgs(graphsToAdd);
		vector<PQLRelationship> all = pkb.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs);

		// then should be inside
		REQUIRE(expectedRelationships.size() == all.size());
		for (int i = 0; i < expectedRelationships.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedRelationships[i]) != all.end());
		}
	};

	SECTION("NextStar") { // TODO: consider refactor to cfgnode


		/*
		procedure p {
		1	x = 1; // assign
		2	call a; // call
		3	if (x == 1) then { // if
		4		read b; // read
			} else {
		5		print x; // print
			}
		6	while (x == 1) { // while
		7		x = 0; // assign
			}
		8	x = 1;
		}
		*/

		// graph 
		unordered_map<int, vector<int>> adjList = {
			{1, {2}},
			{2, {3}},
			{3, {4,5}},
			{4, {6}},
			{5, {6}},
			{6, {7,8}},
			{7, {6}},

		};
		unordered_map<int, shared_ptr<CFGNode>> nodeIdToNode = {
			{1, CFGNode::createCFGNode(Entity::createAssignEntity(1))},
			{2, CFGNode::createCFGNode(Entity::createCallEntity(2))},
			{3, CFGNode::createCFGNode(Entity::createIfEntity(3))},
			{4, CFGNode::createCFGNode(Entity::createReadEntity(4))},
			{5, CFGNode::createCFGNode(Entity::createPrintEntity(5))},
			{6, CFGNode::createCFGNode(Entity::createWhileEntity(6))},
			{7, CFGNode::createCFGNode(Entity::createAssignEntity(7))},
			{8, CFGNode::createCFGNode(Entity::createAssignEntity(8))},
		};

		vector<shared_ptr<CFGNode>> graph = { CFGNode::createCFGFromAdjacencyList(nodeIdToNode, adjList, 1) };
		
		// shared, as PQLEntities
		PQLEntity procedureResult = PQLEntity::generateProcedure("p");
		PQLEntity xResult = PQLEntity::generateVariable("x");

		PQLEntity statementResult1 = PQLEntity::generateStatement(1);
		PQLEntity statementResult2 = PQLEntity::generateStatement(2);
		PQLEntity statementResult3 = PQLEntity::generateStatement(3);
		PQLEntity statementResult4 = PQLEntity::generateStatement(4);
		PQLEntity statementResult5 = PQLEntity::generateStatement(5);
		PQLEntity statementResult6 = PQLEntity::generateStatement(6);
		PQLEntity statementResult7 = PQLEntity::generateStatement(7);
		PQLEntity statementResult8 = PQLEntity::generateStatement(8);

		// test 1a: both exact but inside
		ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
		ClauseArgument rhs = ClauseArgument::createLineNumberArg("2");
		vector<PQLRelationship> expectedRelationships = {
			PQLRelationship(statementResult1, statementResult2),
		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 1b: both exact but one not inside
		lhs = ClauseArgument::createLineNumberArg("4");
		rhs = ClauseArgument::createLineNumberArg("3");
		expectedRelationships = {
		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 2: both wild card
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			// 1 onwards
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult5),
			PQLRelationship(statementResult1, statementResult6),
			PQLRelationship(statementResult1, statementResult7),
			PQLRelationship(statementResult1, statementResult8),

			// 2 onwards
			PQLRelationship(statementResult2, statementResult3),
			PQLRelationship(statementResult2, statementResult4),
			PQLRelationship(statementResult2, statementResult5),
			PQLRelationship(statementResult2, statementResult6),
			PQLRelationship(statementResult2, statementResult7),
			PQLRelationship(statementResult2, statementResult8),

			// 3 onwards
			PQLRelationship(statementResult3, statementResult4),
			PQLRelationship(statementResult3, statementResult5),
			PQLRelationship(statementResult3, statementResult6),
			PQLRelationship(statementResult3, statementResult7),
			PQLRelationship(statementResult3, statementResult8),

			// 4 onwards
			PQLRelationship(statementResult4, statementResult6),
			PQLRelationship(statementResult4, statementResult7),
			PQLRelationship(statementResult4, statementResult8),

			// 5 onwards
			PQLRelationship(statementResult5, statementResult6),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult5, statementResult8),

			// 6 onwards
			PQLRelationship(statementResult6, statementResult6), // while loop self loop
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult6, statementResult8),

			// 7 onwards
			PQLRelationship(statementResult7, statementResult6), // while loop self loop
			PQLRelationship(statementResult7, statementResult7), // while loop self loop
			PQLRelationship(statementResult7, statementResult8),

		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 3: exact then wildcard
		lhs = ClauseArgument::createLineNumberArg("1");
		rhs = ClauseArgument::createStmtArg("s");
		expectedRelationships = {
			// 1 onwards
			PQLRelationship(statementResult1, statementResult2),
			PQLRelationship(statementResult1, statementResult3),
			PQLRelationship(statementResult1, statementResult4),
			PQLRelationship(statementResult1, statementResult5),
			PQLRelationship(statementResult1, statementResult6),
			PQLRelationship(statementResult1, statementResult7),
			PQLRelationship(statementResult1, statementResult8),
		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);


		// test 4a: wildcard then exact, but exact not inside
		lhs = ClauseArgument::createWildcardArg();
		rhs = ClauseArgument::createLineNumberArg("1");
		expectedRelationships = {};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 4b: wildcard then exact, but exact is inside
		lhs = ClauseArgument::createStmtArg("s");
		rhs = ClauseArgument::createLineNumberArg("7");
		expectedRelationships = {
			// 1 onwards
			PQLRelationship(statementResult1, statementResult7),
			PQLRelationship(statementResult2, statementResult7),
			PQLRelationship(statementResult3, statementResult7),
			PQLRelationship(statementResult4, statementResult7),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult7, statementResult7),

		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 4c: wildcard then typed
		lhs = ClauseArgument::createStmtArg("s");
		rhs = ClauseArgument::createAssignArg("a");
		expectedRelationships = {
			PQLRelationship(statementResult1, statementResult7),
			PQLRelationship(statementResult1, statementResult8),
			PQLRelationship(statementResult2, statementResult7),
			PQLRelationship(statementResult2, statementResult8),
			PQLRelationship(statementResult3, statementResult7),
			PQLRelationship(statementResult3, statementResult8),
			PQLRelationship(statementResult4, statementResult7),
			PQLRelationship(statementResult4, statementResult8),
			PQLRelationship(statementResult5, statementResult7),
			PQLRelationship(statementResult5, statementResult8),
			PQLRelationship(statementResult6, statementResult7),
			PQLRelationship(statementResult6, statementResult8),
			PQLRelationship(statementResult7, statementResult7), // while loop self loop
			PQLRelationship(statementResult7, statementResult8),

		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);

		// test 4d: typed then typed
		lhs = ClauseArgument::createCallArg("c");
		rhs = ClauseArgument::createAssignArg("a");
		expectedRelationships = {
			PQLRelationship(statementResult2, statementResult7),
			PQLRelationship(statementResult2, statementResult8),

		};
		test(PKBTrackedRelationshipType::NEXTSTAR, lhs, rhs, expectedRelationships, graph);
	}

}

TEST_CASE("Add and get graph+table relationships (e.g. affects) by type and lhs and rhs") {
	auto test = [](
		PKBTrackedRelationshipType relationshipType, 
		ClauseArgument lhs, 
		ClauseArgument rhs, 
		vector<PQLRelationship> expectedRelationships, 
		vector<shared_ptr<CFGNode>> graphsToAdd,
		vector<Relationship> relationshipsToAdd,
		vector<Entity> entitiesToAdd
	) {
		// given pkb 
		PKB pkb;

		// when add and retrieve relationships 
		pkb.addCfgs(graphsToAdd);
		pkb.addRelationships(relationshipsToAdd);
		pkb.addEntities(entitiesToAdd);
		vector<PQLRelationship> all = pkb.retrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs);

		// then should be inside
		REQUIRE(expectedRelationships.size() == all.size());
		for (int i = 0; i < expectedRelationships.size(); i++) {
			// retrieval all have been added
			REQUIRE(find(all.begin(), all.end(), expectedRelationships[i]) != all.end());
		}
	};

	SECTION("Affects: both exact") {

		/*
		procedure Second {
01			x = 0;
02			i = 5;
03			while (i!=0) {
04				x = x + 2*y;
05				call Third;
06				i = i - 1; }
07			if (x==1) then {
08			   x = x+1; }
			else {
09				  z = 1; }
10			z = z + x + i;
11			y = z + 2;
12			x = x * y + z; 
		}
		*/

		// create entities
		// statements
		Entity line1 = Entity::createAssignEntity(1);
		Entity line2 = Entity::createAssignEntity(2);
		Entity line3 = Entity::createWhileEntity(3);
		Entity line4 = Entity::createAssignEntity(4);
		Entity line5 = Entity::createCallEntity(5);
		Entity line6 = Entity::createAssignEntity(6);
		Entity line7 = Entity::createIfEntity(7);
		Entity line8 = Entity::createAssignEntity(8);
		Entity line9 = Entity::createAssignEntity(9);
		Entity line10 = Entity::createAssignEntity(10);
		Entity line11 = Entity::createAssignEntity(11);
		Entity line12 = Entity::createAssignEntity(12);

		// variables
		Entity xVariable = Entity::createVariableEntity(-1, Token::createNameOrKeywordToken("x"));
		Entity iVariable = Entity::createVariableEntity(-1, Token::createNameOrKeywordToken("i"));
		Entity yVariable = Entity::createVariableEntity(-1, Token::createNameOrKeywordToken("y"));
		Entity zVariable = Entity::createVariableEntity(-1, Token::createNameOrKeywordToken("z"));

		vector<Entity> entitiesToAdd = {
			line1, line2, line3, line4, line5, line6, line7, line8, line9, line10, line12,
			xVariable, iVariable, yVariable, zVariable
		};


		// create relationships
		vector<Relationship> relationshipsToAdd = {
			// uses (only assign, variable)
			Relationship::createUsesRelationship(line4, xVariable),
			Relationship::createUsesRelationship(line4, yVariable),
			Relationship::createUsesRelationship(line6, iVariable),
			Relationship::createUsesRelationship(line8, xVariable),
			Relationship::createUsesRelationship(line10, zVariable),
			Relationship::createUsesRelationship(line10, xVariable),
			Relationship::createUsesRelationship(line11, zVariable),
			Relationship::createUsesRelationship(line12, xVariable),
			Relationship::createUsesRelationship(line12, yVariable),
			Relationship::createUsesRelationship(line12, zVariable),

			// modifies (only assign, variables)
			Relationship::createModifiesRelationship(line1, xVariable),
			Relationship::createModifiesRelationship(line2, iVariable),
			Relationship::createModifiesRelationship(line4, xVariable),
			Relationship::createModifiesRelationship(line6, iVariable),
			Relationship::createModifiesRelationship(line8, xVariable),
			Relationship::createModifiesRelationship(line9, zVariable),
			Relationship::createModifiesRelationship(line10, zVariable),
			Relationship::createModifiesRelationship(line11, yVariable),
			Relationship::createModifiesRelationship(line12, xVariable),
			// next
			Relationship::createNextRelationship(line1, line2),
			Relationship::createNextRelationship(line2, line3),
			Relationship::createNextRelationship(line3, line4),
			Relationship::createNextRelationship(line4, line5),
			Relationship::createNextRelationship(line5, line6),
			Relationship::createNextRelationship(line6, line3),
			Relationship::createNextRelationship(line6, line7),
			Relationship::createNextRelationship(line7, line8),
			Relationship::createNextRelationship(line7, line9),
			Relationship::createNextRelationship(line8, line10),
			Relationship::createNextRelationship(line9, line10),
			Relationship::createNextRelationship(line10, line11),
			Relationship::createNextRelationship(line11, line12),
		};
		
		// create graphs
		unordered_map<int, vector<int>> procedure2AdjList = {
			{1, {2}},
			{2, {3}},
			{3, {4}},
			{4, {5}},
			{5, {6}},
			{6, {3, 7}},
			{7, {8,9}},
			{8, {10}},
			{9, {10}},
			{10, {11}},
			{11, {12}},
			{12, {}},

		};
		unordered_map<int, shared_ptr<CFGNode>> procedure2NodeIdToNode = {
			{1, CFGNode::createCFGNode(line1)},
			{2, CFGNode::createCFGNode(line2)},
			{3, CFGNode::createCFGNode(line3)},
			{4, CFGNode::createCFGNode(line4)},
			{5, CFGNode::createCFGNode(line5)},
			{6, CFGNode::createCFGNode(line6)},
			{7, CFGNode::createCFGNode(line7)},
			{8, CFGNode::createCFGNode(line8)},
			{9, CFGNode::createCFGNode(line9)},
			{10, CFGNode::createCFGNode(line10)},
			{11, CFGNode::createCFGNode(line11)},
			{12, CFGNode::createCFGNode(line12)},
		};
		vector<shared_ptr<CFGNode>> graphsToAdd = {
			CFGNode::createCFGFromAdjacencyList(procedure2NodeIdToNode, procedure2AdjList, 1) 
		};


		SECTION("Referenced statement is not assign, empty result") {
			ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
			ClauseArgument rhs = ClauseArgument::createLineNumberArg("3");
			vector<PQLRelationship> expectedRelationships = {};
			test(PKBTrackedRelationshipType::AFFECTS, lhs, rhs, expectedRelationships, graphsToAdd, relationshipsToAdd, entitiesToAdd);
		}

		SECTION("Exact statements are not inside the program") {
			ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
			ClauseArgument rhs = ClauseArgument::createLineNumberArg("13");
			vector<PQLRelationship> expectedRelationships = {};
			test(PKBTrackedRelationshipType::AFFECTS, lhs, rhs, expectedRelationships, graphsToAdd, relationshipsToAdd, entitiesToAdd);

		}

		SECTION("Exact statemetns are in the program, and do affects, 1") {
			ClauseArgument lhs = ClauseArgument::createLineNumberArg("4");
			ClauseArgument rhs = ClauseArgument::createLineNumberArg("10");
			vector<PQLRelationship> expectedRelationships = {
				PQLRelationship(PQLEntity::generateStatement(4), PQLEntity::generateStatement(10))
			};
			test(PKBTrackedRelationshipType::AFFECTS, lhs, rhs, expectedRelationships, graphsToAdd, relationshipsToAdd, entitiesToAdd);

		}


		SECTION("Exact statemetns are in the program, and do affects, 2") {
			ClauseArgument lhs = ClauseArgument::createLineNumberArg("4");
			ClauseArgument rhs = ClauseArgument::createLineNumberArg("8");
			vector<PQLRelationship> expectedRelationships = {
				PQLRelationship(PQLEntity::generateStatement(4), PQLEntity::generateStatement(8)) 
			};
			test(PKBTrackedRelationshipType::AFFECTS, lhs, rhs, expectedRelationships, graphsToAdd, relationshipsToAdd, entitiesToAdd);

		}

		SECTION("Exact statements are in the program, but do affects, 3") {
			ClauseArgument lhs = ClauseArgument::createLineNumberArg("2");
			ClauseArgument rhs = ClauseArgument::createLineNumberArg("6");
			vector<PQLRelationship> expectedRelationships = {
				PQLRelationship(PQLEntity::generateStatement(2), PQLEntity::generateStatement(6))
			};
			test(PKBTrackedRelationshipType::AFFECTS, lhs, rhs, expectedRelationships, graphsToAdd, relationshipsToAdd, entitiesToAdd);
		}

		SECTION("Exact statements are in the program, but no affects") {
			ClauseArgument lhs = ClauseArgument::createLineNumberArg("1");
			ClauseArgument rhs = ClauseArgument::createLineNumberArg("2");
			vector<PQLRelationship> expectedRelationships = {
				
			};
			test(PKBTrackedRelationshipType::AFFECTS, lhs, rhs, expectedRelationships, graphsToAdd, relationshipsToAdd, entitiesToAdd);
		}

		

		
	}

}





