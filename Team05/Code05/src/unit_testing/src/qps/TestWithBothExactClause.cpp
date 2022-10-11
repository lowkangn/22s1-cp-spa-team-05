#include "catch.hpp"
#include <qps/query/clause/WithBothExactClause.h>
#include <qps/query/clause/WithOneExactClause.h>
#include <qps/query/clause/WithNoExactClause.h>
#include <pkb/PKB.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("WithBothExactClause: test equals") {
	auto testEquals = [](shared_ptr<WithClause> first, shared_ptr<WithClause> second, bool expectedIsEqual) {
		// given

		// when
		bool actualIsEqual = first->equals(second);

		// then
		REQUIRE(actualIsEqual == expectedIsEqual);
	};

	ClauseArgument firstNumberArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument secondNumberArg = ClauseArgument::createLineNumberArg("2");
	ClauseArgument firstStringArg = ClauseArgument::createStringLiteralArg("someName");
	ClauseArgument secondStringArg = ClauseArgument::createStringLiteralArg("otherName");

	ClauseArgument procedureArg = ClauseArgument::createProcedureArg("someName");
	ClauseArgument callsArg = ClauseArgument::createCallArg("c1");
	ClauseArgument procNameArg = ClauseArgument::createProcNameAttributeArg(callsArg);

	shared_ptr<WithClause> first;
	shared_ptr<WithClause> second;
	SECTION("Equal") {
		first = make_shared<WithBothExactClause>(firstNumberArg, secondNumberArg);
		second = make_shared<WithBothExactClause>(firstNumberArg, secondNumberArg);
		testEquals(first, second, true);

		first = make_shared<WithBothExactClause>(firstStringArg, secondStringArg);
		second = make_shared<WithBothExactClause>(firstStringArg, secondStringArg);
		testEquals(first, second, true);
	}

	SECTION("Not equal - different type of with clause") {
		first = make_shared<WithBothExactClause>(firstStringArg, secondStringArg);
		vector<ClauseArgument> rhsArgs = { procedureArg, procNameArg };
		second = make_shared<WithOneExactClause>(firstStringArg, rhsArgs);
		testEquals(first, second, false);

		vector<ClauseArgument> lhsArgs = { callsArg, procNameArg };
		second = make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
		testEquals(first, second, false);
	}
}

// =============== INTEGRATION TESTS ====================
TEST_CASE("WithBothExactClause: test execute") {
	auto testExecute = [](WithBothExactClause withBothExactClause,
		EntityClauseResult expectedClauseResult,
		shared_ptr<PKBQueryHandler> pkb) {

			// when
			shared_ptr<ClauseResult> resPtr = withBothExactClause.execute(pkb);
			shared_ptr<EntityClauseResult> entityResPtr = dynamic_pointer_cast<EntityClauseResult>(resPtr);

			// then
			REQUIRE(*entityResPtr == expectedClauseResult);

	};


	// ------ PKB ------ 
	shared_ptr<PKBQueryHandler> pkb = shared_ptr<PKBQueryHandler>(new PKB());

	// ------ QPS ------ 
	
	PQLEntity stmt1Entity = PQLEntity::generateStatement(1);
	PQLEntity stmt10Entity = PQLEntity::generateStatement(10);
	ClauseArgument integer1Arg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument integer10Arg = ClauseArgument::createLineNumberArg("10");

	PQLEntity firstVariableEntity = PQLEntity::generateVariable("first");
	PQLEntity secondVariableEntity = PQLEntity::generateVariable("notfirst");
	ClauseArgument firstStringArg = ClauseArgument::createStringLiteralArg("first");
	ClauseArgument secondStringArg = ClauseArgument::createStringLiteralArg("notfirst");

	SECTION("Non-empty results") {
		WithBothExactClause clause = WithBothExactClause(integer1Arg, integer1Arg);
		vector<PQLEntity> expectedEntityVector = { stmt1Entity };
		EntityClauseResult expectedClauseResult = EntityClauseResult(integer1Arg, expectedEntityVector);
		testExecute(clause, expectedClauseResult, pkb);

		clause = WithBothExactClause(secondStringArg, secondStringArg);
		expectedEntityVector = { secondVariableEntity };
		expectedClauseResult = EntityClauseResult(secondStringArg, expectedEntityVector);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Empty results") {
		WithBothExactClause clause = WithBothExactClause(firstStringArg, secondStringArg);
		vector<PQLEntity> expectedEntityVector = { };
		EntityClauseResult expectedClauseResult = EntityClauseResult(firstStringArg, expectedEntityVector);
		testExecute(clause, expectedClauseResult, pkb);

		clause = WithBothExactClause(integer1Arg, integer10Arg);
		expectedClauseResult = EntityClauseResult(integer1Arg, expectedEntityVector);
		testExecute(clause, expectedClauseResult, pkb);
	}
}
