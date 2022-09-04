#include "catch.hpp"
#include <pkb/design_objects/PkbEntity.h>
#include <pkb/design_objects/PkbRelationship.h>
#include <pkb/table_managers/EntityPkbTableManager.h>
#include <pkb/table_managers/EntityPkbTableManager.cpp>
#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <pkb/table_managers/ModifiesPkbTableManager.cpp>

TEST_CASE("ModifiesPkbTableManager: test add") {

	auto testAdd = [](EntityPkbTableManager entityManager,
		vector<PkbRelationship> relationships, vector<int> expectedResult) {

		REQUIRE(relationships.size() == expectedResult.size());

		ModifiesPkbTableManager modifiesManager = ModifiesPkbTableManager(entityManager);

		for (int i = 0; i < relationships.size(); i++) {

			int actualResult = modifiesManager.add(relationships[i]);

			REQUIRE(actualResult == expectedResult[i]);
		}
	};

	EntityPkbTableManager entityManager = EntityPkbTableManager();

	PkbEntity varX = PkbEntity::generateVariable("x");
	PkbEntity varY = PkbEntity::generateVariable("y");

	PkbEntity readX = PkbEntity::generateStatement("read x", 1);
	PkbRelationship readModifiesX = PkbRelationship(entityManager.add(readX), entityManager.add(varX));

	PkbEntity readY = PkbEntity::generateStatement("read y", 2);
	PkbRelationship readModifiesY = PkbRelationship(entityManager.add(readY), entityManager.add(varY));

	PkbEntity assignX = PkbEntity::generateStatement("x = x + 1;", 3);
	PkbRelationship assignModifiesX = PkbRelationship(entityManager.add(assignX), entityManager.add(varX));

	PkbEntity assignY = PkbEntity::generateStatement("y = y + 2", 4);
	PkbRelationship assignModifiesY = PkbRelationship(entityManager.add(assignY), entityManager.add(varY));
	
	testAdd(entityManager, { readModifiesX, readModifiesY, assignModifiesX, assignModifiesY }, { 0, 1, 2, 3 });
}

TEST_CASE("ModifiesPkbTableManager: test filter") {
	auto testFilter = [](ModifiesPkbTableManager modifiesManager, PkbClause clause, vector<int> expectedResult) {

		vector<int> actualResults = modifiesManager.filter(clause);

		REQUIRE(actualResults.size() == expectedResult.size());

		for (int i = 0; i < actualResults.size(); i++) {
			REQUIRE(actualResults[i] == expectedResult[i]);
		}
	};

	EntityPkbTableManager entityManager = EntityPkbTableManager();
	ModifiesPkbTableManager modifiesManager = ModifiesPkbTableManager(entityManager);

	int x_id = entityManager.add(PkbEntity::generateVariable("x"));
	int y_id = entityManager.add(PkbEntity::generateVariable("y"));
	
	int readX_id = entityManager.add(PkbEntity::generateStatement("read x", 1));
	int readY_id = entityManager.add(PkbEntity::generateStatement("read y", 3));
	int assignY_id = entityManager.add(PkbEntity::generateStatement("y = y + 1;", 2));
	
	modifiesManager.add(PkbRelationship(readX_id, x_id));
	modifiesManager.add(PkbRelationship(readY_id, y_id));
	modifiesManager.add(PkbRelationship(assignY_id, y_id));

	SECTION("Filter by valid LHS") {
		testFilter(modifiesManager, PkbClause("1", ""), { x_id });
		testFilter(modifiesManager, PkbClause("3", ""), { y_id });
		testFilter(modifiesManager, PkbClause("2", ""), { y_id });
	};

	SECTION("Filter by invalid LHS") {
		testFilter(modifiesManager, PkbClause("5", ""), { });
	}

	SECTION("Filter by valid RHS") {
		testFilter(modifiesManager, PkbClause("", "x"), { readX_id });
		testFilter(modifiesManager, PkbClause("", "y"), { readY_id, assignY_id });
	};

	SECTION("Filter by invalid RHS") {
		testFilter(modifiesManager, PkbClause("", "a"), { });
	}
}

TEST_CASE("ModifiesPkbTableManager: test clearDataBase") {
	auto testClearDataBase = []() {

		EntityPkbTableManager entityManager = EntityPkbTableManager();
		ModifiesPkbTableManager modifiesManager = ModifiesPkbTableManager(entityManager);

		PkbEntity varX = PkbEntity::generateVariable("x");
		PkbEntity varY = PkbEntity::generateVariable("y");

		PkbEntity readX = PkbEntity::generateStatement("read x", 1);
		modifiesManager.add(PkbRelationship(entityManager.add(readX), entityManager.add(varX)));

		PkbEntity assignY = PkbEntity::generateStatement("y = y + 1;", 3);
		modifiesManager.add(PkbRelationship(entityManager.add(assignY), entityManager.add(varY)));

		modifiesManager.clearDataBase();

		PkbEntity varA = PkbEntity::generateVariable("a");
		PkbEntity readA = PkbEntity::generateStatement("read a", 1);

		REQUIRE(modifiesManager.add(PkbRelationship(entityManager.add(readA), entityManager.add(varA))) == 0);
	};

	testClearDataBase();
}
