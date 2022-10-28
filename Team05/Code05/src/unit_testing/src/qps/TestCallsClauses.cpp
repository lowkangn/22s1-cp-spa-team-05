#include "catch.hpp"
#include <qps/query/clause/CallsClause.h>
#include <qps/query/clause/CallsTClause.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsStarRelationship.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("CallsClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p1");
		ClauseArgument rhs = ClauseArgument::createProcedureArg("p2");
		CallsClause callsClause = CallsClause(lhs, rhs);

		// when
		bool actual = callsClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcedureArg = ClauseArgument::createProcedureArg("p1");
	ClauseArgument secondProcedureArg = ClauseArgument::createProcedureArg("p2");
	ClauseArgument mainProcedureArg = ClauseArgument::createStringLiteralArg("main");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> callsClauseAgain(new CallsClause(firstProcedureArg, secondProcedureArg));
		testEquals(callsClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> callsClauseDiffLhs(new CallsClause(secondProcedureArg, secondProcedureArg));
		testEquals(callsClauseDiffLhs, false);

		shared_ptr<RelationshipClause> callsClauseDiffRhs(new CallsClause(firstProcedureArg, firstProcedureArg));
		testEquals(callsClauseDiffRhs, false);

		shared_ptr<RelationshipClause> callsClauseDiffArgType(new CallsClause(firstProcedureArg, mainProcedureArg));
		testEquals(callsClauseDiffArgType, false);
	}

	SECTION("Not equal, CallsClause vs CallsTClause") {
		shared_ptr<RelationshipClause> callsTClause(new CallsTClause(firstProcedureArg, secondProcedureArg));
		testEquals(callsTClause, false);
	}
}

TEST_CASE("CallsTClause: test equals") {
	auto testEquals = [](const shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createProcedureArg("p1");
		ClauseArgument rhs = ClauseArgument::createProcedureArg("p2");
		CallsTClause callsTClause = CallsTClause(lhs, rhs);

		// when
		bool actual = callsTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstProcedureArg = ClauseArgument::createProcedureArg("p1");
	ClauseArgument secondProcedureArg = ClauseArgument::createProcedureArg("p2");
	ClauseArgument mainProcedureArg = ClauseArgument::createStringLiteralArg("main");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> callsTClauseAgain(new CallsTClause(firstProcedureArg, secondProcedureArg));
		testEquals(callsTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> callsTClauseDiffLhs(new CallsTClause(secondProcedureArg, secondProcedureArg));
		testEquals(callsTClauseDiffLhs, false);

		shared_ptr<RelationshipClause> callsTClauseDiffRhs(new CallsTClause(firstProcedureArg, firstProcedureArg));
		testEquals(callsTClauseDiffRhs, false);

		shared_ptr<RelationshipClause> callsTClauseDiffArgType(new CallsTClause(firstProcedureArg, mainProcedureArg));
		testEquals(callsTClauseDiffArgType, false);
	}

	SECTION("Not equal, CallsClause vs CallsTClause") {
		shared_ptr<RelationshipClause> callsClause(new CallsClause(firstProcedureArg, secondProcedureArg));
		testEquals(callsClause, false);
	}
}

// =============== INTEGRATION TESTS ====================
// this prevents other files from using these variables
namespace {
	/*
			procedure main {
				call alpha;
				call beta;
			}

			procedure alpha {
				call charlie;
			}

			procedure beta {
				call delta;
			}

			procedure charlie {}

			procedure delta {
				call charlie;
			}
		*/

	// Initialise tokens
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token alphaToken = Token::createNameOrKeywordToken("alpha");
	Token betaToken = Token::createNameOrKeywordToken("beta");
	Token charlieToken = Token::createNameOrKeywordToken("charlie");
	Token deltaToken = Token::createNameOrKeywordToken("delta");

	// Initialise procedure entities
	Entity mainEntity = Entity::createProcedureEntity(mainToken);
	Entity alphaEntity = Entity::createProcedureEntity(alphaToken);
	Entity betaEntity = Entity::createProcedureEntity(betaToken);
	Entity charlieEntity = Entity::createProcedureEntity(charlieToken);
	Entity deltaEntity = Entity::createProcedureEntity(deltaToken);

	// Initialise calls relationships
	Relationship callsMainAlpha = Relationship::createCallsRelationship(mainEntity, alphaEntity);
	Relationship callsMainBeta = Relationship::createCallsRelationship(mainEntity, betaEntity);
	Relationship callsAlphaCharlie = Relationship::createCallsRelationship(alphaEntity, charlieEntity);
	Relationship callsBetaDelta = Relationship::createCallsRelationship(betaEntity, deltaEntity);
	Relationship callsDeltaCharlie = Relationship::createCallsRelationship(deltaEntity, charlieEntity);

	Relationship callsTMainAlpha = Relationship::createCallsTRelationship(mainEntity, alphaEntity);
	Relationship callsTMainBeta = Relationship::createCallsTRelationship(mainEntity, betaEntity);
	Relationship callsTAlphaCharlie = Relationship::createCallsTRelationship(alphaEntity, charlieEntity);
	Relationship callsTBetaDelta = Relationship::createCallsTRelationship(betaEntity, deltaEntity);
	Relationship callsTDeltaCharlie = Relationship::createCallsTRelationship(deltaEntity, charlieEntity);

	Relationship callsTMainCharlie = Relationship::createCallsTRelationship(mainEntity, charlieEntity);
	Relationship callsTMainDelta = Relationship::createCallsTRelationship(mainEntity, deltaEntity);
	Relationship callsTBetaCharlie = Relationship::createCallsTRelationship(betaEntity, charlieEntity);

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlMain = PQLEntity::generateProcedure("main");
	PQLEntity pqlAlpha = PQLEntity::generateProcedure("alpha");
	PQLEntity pqlBeta = PQLEntity::generateProcedure("beta");
	PQLEntity pqlCharlie = PQLEntity::generateProcedure("charlie");
	PQLEntity pqlDelta = PQLEntity::generateProcedure("delta");

	PQLRelationship pqlCallsMainAlpha = PQLRelationship(pqlMain, pqlAlpha);
	PQLRelationship pqlCallsMainBeta = PQLRelationship(pqlMain, pqlBeta);
	PQLRelationship pqlCallsAlphaCharlie = PQLRelationship(pqlAlpha, pqlCharlie);
	PQLRelationship pqlCallsBetaDelta = PQLRelationship(pqlBeta, pqlDelta);
	PQLRelationship pqlCallsDeltaCharlie = PQLRelationship(pqlDelta, pqlCharlie);

	PQLRelationship pqlCallsTMainAlpha = PQLRelationship(pqlMain, pqlAlpha);
	PQLRelationship pqlCallsTMainBeta = PQLRelationship(pqlMain, pqlBeta);
	PQLRelationship pqlCallsTAlphaCharlie = PQLRelationship(pqlAlpha, pqlCharlie);
	PQLRelationship pqlCallsTBetaDelta = PQLRelationship(pqlBeta, pqlDelta);
	PQLRelationship pqlCallsTDeltaCharlie = PQLRelationship(pqlDelta, pqlCharlie);

	PQLRelationship pqlCallsTMainCharlie = PQLRelationship(pqlMain, pqlCharlie);
	PQLRelationship pqlCallsTMainDelta = PQLRelationship(pqlMain, pqlDelta);
	PQLRelationship pqlCallsTBetaCharlie = PQLRelationship(pqlBeta, pqlCharlie);

	// Clause Arguments
	ClauseArgument firstProcedureArg = ClauseArgument::createProcedureArg("p1");
	ClauseArgument secondProcedureArg = ClauseArgument::createProcedureArg("p2");
	ClauseArgument mainProcedureArg = ClauseArgument::createStringLiteralArg("main");
	ClauseArgument alphaProcedureArg = ClauseArgument::createStringLiteralArg("alpha");
	ClauseArgument betaProcedureArg = ClauseArgument::createStringLiteralArg("beta");
	ClauseArgument charlieProcedureArg = ClauseArgument::createStringLiteralArg("charlie");
	ClauseArgument deltaProcedureArg = ClauseArgument::createStringLiteralArg("delta");
};

TEST_CASE("CallsClause: test execute") {
	auto testExecute = [](CallsClause callsClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = callsClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};


	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ mainEntity, alphaEntity, betaEntity, charlieEntity, deltaEntity };
	vector<Relationship> relationships{ 
			callsMainAlpha, callsMainBeta, callsAlphaCharlie, callsBetaDelta, callsDeltaCharlie, 
			callsTMainAlpha, callsTMainBeta, callsTAlphaCharlie, callsTBetaDelta, callsTDeltaCharlie, 
			callsTMainCharlie, callsTMainDelta, callsTBetaCharlie };
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	CallsClause clause = CallsClause(firstProcedureArg, secondProcedureArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { 
		pqlCallsMainAlpha, pqlCallsMainBeta, pqlCallsAlphaCharlie, pqlCallsBetaDelta, pqlCallsDeltaCharlie };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstProcedureArg, secondProcedureArg,
		expectedRetrievedFromPkb);

	SECTION("Two procedure synoynms - all calls results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One procedure string literal and one procedure synonym - non empty results") {
		clause = CallsClause(firstProcedureArg, charlieProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsAlphaCharlie, pqlCallsDeltaCharlie };
		expectedClauseResult = RelationshipClauseResult(firstProcedureArg, charlieProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsClause(mainProcedureArg, secondProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsMainAlpha, pqlCallsMainBeta };
		expectedClauseResult = RelationshipClauseResult(mainProcedureArg, secondProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One procedure string literal and one procedure synonym - empty results") {
		expectedRetrievedFromPkb = {};

		clause = CallsClause(firstProcedureArg, mainProcedureArg);
		expectedClauseResult = RelationshipClauseResult(firstProcedureArg, mainProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsClause(charlieProcedureArg, secondProcedureArg);
		expectedClauseResult = RelationshipClauseResult(charlieProcedureArg, secondProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two procedure string literals - non empty results") {
		clause = CallsClause(deltaProcedureArg, charlieProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsDeltaCharlie };
		expectedClauseResult = RelationshipClauseResult(deltaProcedureArg, charlieProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsClause(betaProcedureArg, deltaProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsBetaDelta };
		expectedClauseResult = RelationshipClauseResult(betaProcedureArg, deltaProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two procedure string literals - empty results") {
		expectedRetrievedFromPkb = {};

		clause = CallsClause(alphaProcedureArg, deltaProcedureArg);
		expectedClauseResult = RelationshipClauseResult(alphaProcedureArg, deltaProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsClause(mainProcedureArg, charlieProcedureArg);
		expectedClauseResult = RelationshipClauseResult(mainProcedureArg, charlieProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}

TEST_CASE("CallsTClause: test execute") {
	auto testExecute = [](CallsTClause callsTClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = callsTClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ mainEntity, alphaEntity, betaEntity, charlieEntity, deltaEntity };
	vector<Relationship> relationships{ 
			callsMainAlpha, callsMainBeta, callsAlphaCharlie, callsBetaDelta, callsDeltaCharlie,
			callsTMainAlpha, callsTMainBeta, callsTAlphaCharlie, callsTBetaDelta, callsTDeltaCharlie,
			callsTMainCharlie, callsTMainDelta, callsTBetaCharlie };
	pkb->addEntities(entities);
	pkb->addRelationships(relationships);


	// ------ QPS ------ 
	CallsTClause clause = CallsTClause(firstProcedureArg, secondProcedureArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { 
			pqlCallsTMainAlpha, pqlCallsTMainBeta, pqlCallsTAlphaCharlie, pqlCallsTBetaDelta, pqlCallsTDeltaCharlie,
			pqlCallsTMainCharlie, pqlCallsTMainDelta, pqlCallsTBetaCharlie };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstProcedureArg, secondProcedureArg,
		expectedRetrievedFromPkb);

	SECTION("Two procedure synoynms - all calls results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One procedure string literal and one procedure synonym - non empty results") {
		clause = CallsTClause(firstProcedureArg, charlieProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsTAlphaCharlie, pqlCallsTDeltaCharlie, pqlCallsTMainCharlie, pqlCallsTBetaCharlie };
		expectedClauseResult = RelationshipClauseResult(firstProcedureArg, charlieProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsTClause(mainProcedureArg, secondProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsTMainAlpha, pqlCallsTMainBeta, pqlCallsTMainCharlie, pqlCallsTMainDelta };
		expectedClauseResult = RelationshipClauseResult(mainProcedureArg, secondProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One procedure string literal and one procedure synonym - empty results") {
		expectedRetrievedFromPkb = {};

		clause = CallsTClause(firstProcedureArg, mainProcedureArg);
		expectedClauseResult = RelationshipClauseResult(firstProcedureArg, mainProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsTClause(charlieProcedureArg, secondProcedureArg);
		expectedClauseResult = RelationshipClauseResult(charlieProcedureArg, secondProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two procedure string literals - non empty results") {
		clause = CallsTClause(mainProcedureArg, charlieProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsTMainCharlie };
		expectedClauseResult = RelationshipClauseResult(mainProcedureArg, charlieProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsTClause(betaProcedureArg, charlieProcedureArg);
		expectedRetrievedFromPkb = { pqlCallsTBetaCharlie };
		expectedClauseResult = RelationshipClauseResult(betaProcedureArg, charlieProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two procedure string literals - empty results") {
		expectedRetrievedFromPkb = {};

		clause = CallsTClause(alphaProcedureArg, deltaProcedureArg);
		expectedClauseResult = RelationshipClauseResult(alphaProcedureArg, deltaProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = CallsTClause(betaProcedureArg, alphaProcedureArg);
		expectedClauseResult = RelationshipClauseResult(betaProcedureArg, alphaProcedureArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
}