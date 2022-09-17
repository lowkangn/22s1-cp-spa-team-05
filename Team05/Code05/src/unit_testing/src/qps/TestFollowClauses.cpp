#include "catch.hpp"
#include <qps/query/clause/FollowsClause.h>
#include <qps/query/clause/FollowsTClause.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>

#include <list>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("FollowsClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		FollowsClause followsClause = FollowsClause(lhs, rhs);

		// when
		bool actual = followsClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> followsClauseAgain(new FollowsClause(firstStmtArg, secondStmtArg));
		testEquals(followsClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> followsClauseDiffLhs(new FollowsClause(secondStmtArg, secondStmtArg));
		testEquals(followsClauseDiffLhs, false);

		shared_ptr<RelationshipClause> followsClauseDiffRhs(new FollowsClause(firstStmtArg, firstStmtArg));
		testEquals(followsClauseDiffRhs, false);

		shared_ptr<RelationshipClause> followsClauseDiffArgType(new FollowsClause(firstStmtArg, lineArg));
		testEquals(followsClauseDiffArgType, false);
	}

	SECTION("Not equal, FollowsClause vs FollowsTClause") {
		shared_ptr<RelationshipClause> followsTClause(new FollowsTClause(firstStmtArg, secondStmtArg));
		testEquals(followsTClause, false);
	}
}

TEST_CASE("FollowsTClause: test equals") {
	auto testEquals = [](const shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		FollowsTClause followsTClause = FollowsTClause(lhs, rhs);

		// when
		bool actual = followsTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> followsTClauseAgain(new FollowsTClause(firstStmtArg, secondStmtArg));
		testEquals(followsTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> followsTClauseDiffLhs(new FollowsTClause(secondStmtArg, secondStmtArg));
		testEquals(followsTClauseDiffLhs, false);

		shared_ptr<RelationshipClause> followsTClauseDiffRhs(new FollowsTClause(firstStmtArg, firstStmtArg));
		testEquals(followsTClauseDiffRhs, false);

		shared_ptr<RelationshipClause> followsTClauseDiffArgType(new FollowsTClause(firstStmtArg, lineArg));
		testEquals(followsTClauseDiffArgType, false);
	}

	SECTION("Not equal, FollowsClause vs FollowsTClause") {
		shared_ptr<RelationshipClause> followsClause(new FollowsClause(firstStmtArg, secondStmtArg));
		testEquals(followsClause, false);
	}
}

