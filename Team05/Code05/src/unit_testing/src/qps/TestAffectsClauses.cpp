#include "catch.hpp"

#include <qps/query/clause/AffectsClause.h>
#include <qps/query/clause/AffectsTClause.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbAffectsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbAffectsStarRelationship.h>

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("AffectsClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		AffectsClause nextClause = AffectsClause(lhs, rhs);

		// when
		bool actual = nextClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> nextClauseAgain(new AffectsClause(firstStmtArg, secondStmtArg));
		testEquals(nextClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> nextClauseDiffLhs(new AffectsClause(secondStmtArg, secondStmtArg));
		testEquals(nextClauseDiffLhs, false);

		shared_ptr<RelationshipClause> nextClauseDiffRhs(new AffectsClause(firstStmtArg, firstStmtArg));
		testEquals(nextClauseDiffRhs, false);

		shared_ptr<RelationshipClause> nextClauseDiffArgType(new AffectsClause(firstStmtArg, lineArg));
		testEquals(nextClauseDiffArgType, false);
	}

	SECTION("Not equal, AffectsClause vs AffectsTClause") {
		shared_ptr<RelationshipClause> nextTClause(new AffectsTClause(firstStmtArg, secondStmtArg));
		testEquals(nextTClause, false);
	}
}

TEST_CASE("AffectsTClause: test equals") {
	auto testEquals = [](const shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		AffectsTClause nextTClause = AffectsTClause(lhs, rhs);

		// when
		bool actual = nextTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> nextTClauseAgain(new AffectsTClause(firstStmtArg, secondStmtArg));
		testEquals(nextTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> nextTClauseDiffLhs(new AffectsTClause(secondStmtArg, secondStmtArg));
		testEquals(nextTClauseDiffLhs, false);

		shared_ptr<RelationshipClause> nextTClauseDiffRhs(new AffectsTClause(firstStmtArg, firstStmtArg));
		testEquals(nextTClauseDiffRhs, false);

		shared_ptr<RelationshipClause> nextTClauseDiffArgType(new AffectsTClause(firstStmtArg, lineArg));
		testEquals(nextTClauseDiffArgType, false);
	}

	SECTION("Not equal, AffectsClause vs AffectsTClause") {
		shared_ptr<RelationshipClause> nextClause(new AffectsClause(firstStmtArg, secondStmtArg));
		testEquals(nextClause, false);
	}
}