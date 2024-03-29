#include <catch.hpp>
#include <pkb/pkbRepository/design_objects/entities/PkbConstantEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/table_managers/PkbEntityTable.h>

#include <vector>
#include <string>
#include <assert.h>
using namespace std;

TEST_CASE("PkbEntityTable::add and ::get works correctly") {
	auto test = [](vector<shared_ptr<PkbEntity>> expectedEntities, vector<string> keys) {
		// given
		PkbEntityTable table;

		// when 
		for (shared_ptr<PkbEntity> e : expectedEntities) {
			// add
			table.add(e);
		}

		// then 
		assert(expectedEntities.size() == keys.size());
		for (int i = 0; i < keys.size(); i++) {
			// retrieval by key matches expectation
			REQUIRE(expectedEntities[i]->equals(table.get(keys[i])));
		}
	};

	// create lhs and rhs, to be shared
	string lhsKey = "procedure";
	string rhsKey = "variable";
	shared_ptr<PkbEntity> lhs = shared_ptr<PkbEntity>(new PkbProcedureEntity(lhsKey));
	shared_ptr<PkbEntity> rhs = shared_ptr<PkbEntity>(new PkbVariableEntity(rhsKey));

	// tests
	SECTION("One entry per entity type") {
		vector<shared_ptr<PkbEntity>> relationships = {
			shared_ptr<PkbEntity>(new PkbProcedureEntity("procedure")),
			shared_ptr<PkbEntity>(new PkbVariableEntity("variable")),
			shared_ptr<PkbEntity>(PkbStatementEntity::createAssignStatementEntity(1)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createCallStatementEntity(2)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createIfStatementEntity(3)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createPrintStatementEntity(4)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(5)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createWhileStatementEntity(6)),
			shared_ptr<PkbEntity>(new PkbConstantEntity(1))
		};
		vector<string> expectedKeys = {
			"procedure",
			"variable",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"constant1"
		};
		test(relationships, expectedKeys);
	};
}

TEST_CASE("PkbEntityTable::getAll works correctly") {
	auto test = [](vector<shared_ptr<PkbEntity>> expectedEntities) {
		// given
		PkbEntityTable table;

		// when 
		for (shared_ptr<PkbEntity> e : expectedEntities) {
			// add
			table.add(e);
		}

		// then 
		vector<shared_ptr<PkbEntity>> all = table.getAll();
		REQUIRE(expectedEntities.size() == all.size());
		for (int i = 0; i < expectedEntities.size(); i++) {
			// matches expectation
			REQUIRE(find(all.begin(), all.end(), expectedEntities[i]) != all.end());
		}
	};
	// tests
	SECTION("One entry per entity type") {
		vector<shared_ptr<PkbEntity>> relationships = {
			shared_ptr<PkbEntity>(new PkbProcedureEntity("procedure")),
			shared_ptr<PkbEntity>(new PkbVariableEntity("variable")),
			shared_ptr<PkbEntity>(PkbStatementEntity::createAssignStatementEntity(1)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createCallStatementEntity(2)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createIfStatementEntity(3)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createPrintStatementEntity(4)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(5)),
			shared_ptr<PkbEntity>(PkbStatementEntity::createWhileStatementEntity(6)),
			shared_ptr<PkbEntity>(new PkbConstantEntity(1))
		};
		test(relationships);
	};

}

//TEST_CASE("PkbEntityTable throws when we try to add duplicate items") {
//	auto test = [](vector<shared_ptr<PkbEntity>> alreadyInTable, shared_ptr<PkbEntity> toAdd) {
//		// given
//		PkbEntityTable table;
//		for (shared_ptr<PkbEntity> e : alreadyInTable) {
//			// add
//			table.add(e);
//		}
//		// when & then
//		REQUIRE_THROWS_AS(table.add(toAdd), PkbException);
//
//	};
//
//	SECTION("One entry per entity type") {
//		vector<shared_ptr<PkbEntity>> alreadyInTable = {
//			shared_ptr<PkbEntity>(new PkbProcedureEntity("procedure")),
//			shared_ptr<PkbEntity>(new PkbVariableEntity("variable")),
//			shared_ptr<PkbEntity>(PkbStatementEntity::createAssignStatementEntity(1)),
//			shared_ptr<PkbEntity>(PkbStatementEntity::createCallStatementEntity(2)),
//			shared_ptr<PkbEntity>(PkbStatementEntity::createIfStatementEntity(3)),
//			shared_ptr<PkbEntity>(PkbStatementEntity::createPrintStatementEntity(4)),
//			shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(5)),
//			shared_ptr<PkbEntity>(PkbStatementEntity::createWhileStatementEntity(6)),
//			shared_ptr<PkbEntity>(new PkbConstantEntity(1))
//		};
//		shared_ptr<PkbEntity> toAdd = shared_ptr<PkbEntity>(PkbStatementEntity::createAssignStatementEntity(1));
//		test(alreadyInTable, toAdd);
//	};
//}
