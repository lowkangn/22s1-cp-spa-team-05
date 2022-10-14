#include <catch.hpp>
#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/table_managers/PkbRelationshipTable.h>


#include <vector>
#include <string>
#include <assert.h>
using namespace std;

TEST_CASE("PkbRelationshipTable::add and ::get works correctly") {
	auto test = [](vector<shared_ptr<PkbRelationship>> expectedRelationships, vector<string> keys) {
		// given
		PkbRelationshipTable table;

		// when 
		for (shared_ptr<PkbRelationship> r : expectedRelationships) {
			// add
			table.add(r);
		}

		// then 
		assert(expectedRelationships.size() == keys.size());
		for (int i = 0; i < keys.size(); i++) {
			// retrieval by key matches expectation
			REQUIRE(expectedRelationships[i] == table.get(keys[i]));
		}
	};

	// create lhs and rhs, to be shared
	string lhsKey = "procedure";
	string rhsKey = "variable";
	shared_ptr<PkbEntity> lhs = shared_ptr<PkbEntity>(new PkbProcedureEntity(lhsKey));
	shared_ptr<PkbEntity> rhs = shared_ptr<PkbEntity>(new PkbVariableEntity(rhsKey));

	// tests
	SECTION("One entry per relationship type") {
		vector<shared_ptr<PkbRelationship>> relationships = {
			shared_ptr<PkbRelationship>(new PkbFollowsRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbParentRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbParentStarRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbUsesRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbCallsRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbCallsStarRelationship(lhs, rhs)),
			
		};
		vector<string> expectedKeys = {
			lhsKey + rhsKey + string("0"),
			lhsKey + rhsKey + string("1"),
			lhsKey + rhsKey + string("2"),
			lhsKey + rhsKey + string("3"),
			lhsKey + rhsKey + string("4"),
			lhsKey + rhsKey + string("5"),
			lhsKey + rhsKey + string("6"),
			lhsKey + rhsKey + string("7"),
		};
		test(relationships, expectedKeys);
	};
}

TEST_CASE("PkbRelationshipTable::getAll works correctly") {
	auto test = [](vector<shared_ptr<PkbRelationship>> expectedRelationships) {
		// given
		PkbRelationshipTable table;

		// when 
		for (shared_ptr<PkbRelationship> r : expectedRelationships) {
			// add
			table.add(r);
		}

		// then 
		vector<shared_ptr<PkbRelationship>> all = table.getAll();
		REQUIRE(expectedRelationships.size() == all.size());
		for (int i = 0; i < expectedRelationships.size(); i++) {
			// matches expectation
			REQUIRE(find(all.begin(), all.end(), expectedRelationships[i]) != all.end());
		}
	};

	// create lhs and rhs, to be shared
	string lhsKey = "procedure";
	string rhsKey = "variable";
	shared_ptr<PkbEntity> lhs = shared_ptr<PkbEntity>(new PkbProcedureEntity(lhsKey));
	shared_ptr<PkbEntity> rhs = shared_ptr<PkbEntity>(new PkbVariableEntity(rhsKey));

	// tests
	SECTION("One entry per relationship type") {
		vector<shared_ptr<PkbRelationship>> relationships = {
			shared_ptr<PkbRelationship>(new PkbFollowsRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbParentRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbParentStarRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbUsesRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbCallsRelationship(lhs, rhs)),
			shared_ptr<PkbRelationship>(new PkbCallsStarRelationship(lhs, rhs)),
		};
		test(relationships);
	};

}

//TEST_CASE("PkbRelationshipTable throws when we try to add duplicate items") {
//	auto test = [](vector<shared_ptr<PkbRelationship>> alreadyInTable, shared_ptr<PkbRelationship> toAdd) {
//		// given
//		PkbRelationshipTable table;
//		for (shared_ptr<PkbRelationship> e : alreadyInTable) {
//			// add
//			table.add(e);
//		}
//		// when & then
//		REQUIRE_THROWS_AS(table.add(toAdd), PkbException);
//
//	};
//
//	// create lhs and rhs, to be shared
//	string lhsKey = "procedure";
//	string rhsKey = "variable";
//	shared_ptr<PkbEntity> lhs = shared_ptr<PkbEntity>(new PkbProcedureEntity(lhsKey));
//	shared_ptr<PkbEntity> rhs = shared_ptr<PkbEntity>(new PkbVariableEntity(rhsKey));
//
//	SECTION("One entry per entity type") {
//		vector<shared_ptr<PkbRelationship>> alreadyInTable = {
//			shared_ptr<PkbRelationship>(new PkbFollowsRelationship(lhs, rhs)),
//			shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(lhs, rhs)),
//			shared_ptr<PkbRelationship>(new PkbParentRelationship(lhs, rhs)),
//			shared_ptr<PkbRelationship>(new PkbParentStarRelationship(lhs, rhs)),
//			shared_ptr<PkbRelationship>(new PkbUsesRelationship(lhs, rhs)),
//			shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhs, rhs))
//		};
//		shared_ptr<PkbRelationship> toAdd = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhs, rhs));
//		test(alreadyInTable, toAdd);
//	};
//}
