#include "catch.hpp"
#include <pkb/design_objects/PkbEntity.h>
#include <pkb/table_managers/EntityPkbTableManager.h>
#include <pkb/table_managers/EntityPkbTableManager.cpp>

using namespace std;

TEST_CASE("EntityPkbTableManager: test add") {

	auto testAdd = [](vector<PkbEntity> entities, vector<int> expectedResult) {

		REQUIRE(entities.size() == expectedResult.size());

		EntityPkbTableManager entityManager = EntityPkbTableManager();

		for (int i = 0; i < entities.size(); i++) {

			int actualResult = entityManager.add(entities[i]);

			REQUIRE(actualResult == expectedResult[i]);
		}
	};

	PkbEntityGenerator g = PkbEntityGenerator();

	PkbEntity varA = g.generateVariable("a");
	PkbEntity varB = g.generateVariable("b");
	PkbEntity varC = g.generateVariable("c");
	PkbEntity varX = g.generateVariable("x");
	PkbEntity varY = g.generateVariable("y");
	PkbEntity readX = g.generateStatement("read x", 1);
	PkbEntity readY = g.generateStatement("read y", 3);
	PkbEntity assignX = g.generateStatement("x = x + 1;", 2);

	SECTION("Single add") {
		testAdd({ varA }, { 0 });
		testAdd({ readX }, { 0 });
	}

	SECTION("Multiple adds") {
		testAdd({ varA, varB, varC }, { 0, 1, 2 });
		testAdd({ readX, varX }, { 0, 1 });
		testAdd({ readX, assignX, readY }, { 0, 1, 2 });
	}

	SECTION("Adding same entities") {

		PkbEntity varACopy = g.generateVariable("a");
		PkbEntity varBCopy = g.generateVariable("b");
		PkbEntity varXCopy = g.generateVariable("x");
		PkbEntity readXCopy = g.generateStatement("read x", 1);

		testAdd({ varA, varB, varC, varACopy, varBCopy }, { 0, 1, 2, 0, 1 });
		testAdd({ varX, readX, readXCopy, varXCopy }, { 0, 1, 1, 0 });
	}

	SECTION("Adding similar statements with different line numbers") {

		PkbEntity readX2 = g.generateStatement("read x", 2);
		PkbEntity readX3 = g.generateStatement("read x", 3);

		testAdd({ readX, readX2, readX3 }, { 0, 1, 2 });
	}
}

TEST_CASE("EntityPkbTableManager: test filter") {

	auto testFilter = [](EntityPkbTableManager entityManager, vector<int> ids, vector<PkbEntity> expectedResult) {

		REQUIRE(ids.size() == expectedResult.size());

		vector<PkbEntity> actualResults = entityManager.filter(ids);

		REQUIRE(actualResults.size() == expectedResult.size());

		for (int i = 0; i < actualResults.size(); i++) {
			actualResults[i] == expectedResult[i];
		}
	};

	PkbEntityGenerator g = PkbEntityGenerator();

	PkbEntity varA = g.generateVariable("a");
	PkbEntity varX = g.generateVariable("x");
	PkbEntity varY = g.generateVariable("y");
	PkbEntity readX = g.generateStatement("read x", 1);
	PkbEntity readY = g.generateStatement("read y", 3);


	EntityPkbTableManager entityManager = EntityPkbTableManager();

	entityManager.add(varX);
	entityManager.add(varY);
	entityManager.add(varA);
	entityManager.add(readX);
	entityManager.add(readY);

	testFilter(entityManager, { 0, 1, 2, 3, 4 }, { varX, varY, varA, readX, readY });
	testFilter(entityManager, { 4, 3, 1 }, { readY, readX, varY });
	testFilter(entityManager, { 1 }, { varY });
}

TEST_CASE("EntityPkbTableManager: test clearDataBase") {

	auto testClear = []() {
		EntityPkbTableManager entityManager = EntityPkbTableManager();
		PkbEntityGenerator g = PkbEntityGenerator();

		entityManager.add(g.generateVariable("a"));
		entityManager.add(g.generateVariable("b"));
		entityManager.add(g.generateVariable("c"));
		entityManager.add(g.generateStatement("read x", 1));
		entityManager.add(g.generateStatement("read y", 2));

		entityManager.clearDataBase();

		REQUIRE(entityManager.add(g.generateVariable("a")) == 0);
	};

	testClear();
}