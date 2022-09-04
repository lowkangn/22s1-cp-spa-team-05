#include "catch.hpp"
#include <pkb/design_objects/PkbEntity.h>
#include <pkb/table_managers/EntityPkbTableManager.h>

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

	PkbEntity varA = PkbEntity::generateVariable("a");
	PkbEntity varB = PkbEntity::generateVariable("b");
	PkbEntity varC = PkbEntity::generateVariable("c");
	PkbEntity varX = PkbEntity::generateVariable("x");
	PkbEntity varY = PkbEntity::generateVariable("y");
	PkbEntity readX = PkbEntity::generateStatement("read x", 1);
	PkbEntity readY = PkbEntity::generateStatement("read y", 3);
	PkbEntity assignX = PkbEntity::generateStatement("x = x + 1;", 2);

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

		PkbEntity varACopy = PkbEntity::generateVariable("a");
		PkbEntity varBCopy = PkbEntity::generateVariable("b");
		PkbEntity varXCopy = PkbEntity::generateVariable("x");
		PkbEntity readXCopy = PkbEntity::generateStatement("read x", 1);

		testAdd({ varA, varB, varC, varACopy, varBCopy }, { 0, 1, 2, 0, 1 });
		testAdd({ varX, readX, readXCopy, varXCopy }, { 0, 1, 1, 0 });
	}

	SECTION("Adding similar statements with different line numbers") {

		PkbEntity readX2 = PkbEntity::generateStatement("read x", 2);
		PkbEntity readX3 = PkbEntity::generateStatement("read x", 3);

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

	PkbEntity varA = PkbEntity::generateVariable("a");
	PkbEntity varX = PkbEntity::generateVariable("x");
	PkbEntity varY = PkbEntity::generateVariable("y");
	PkbEntity readX = PkbEntity::generateStatement("read x", 1);
	PkbEntity readY = PkbEntity::generateStatement("read y", 3);


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

		entityManager.add(PkbEntity::generateVariable("a"));
		entityManager.add(PkbEntity::generateVariable("b"));
		entityManager.add(PkbEntity::generateVariable("c"));
		entityManager.add(PkbEntity::generateStatement("read x", 1));
		entityManager.add(PkbEntity::generateStatement("read y", 2));

		entityManager.clearDataBase();

		REQUIRE(entityManager.add(PkbEntity::generateVariable("a")) == 0);
	};

	testClear();
}

TEST_CASE("EntityPkbTableManager: test findStatement") {
	auto testFindStatement = [](int lineNumber, int expectedResult) {

		EntityPkbTableManager entityManager = EntityPkbTableManager();

		entityManager.add(PkbEntity::generateStatement("x = x + 1;", 8));
		entityManager.add(PkbEntity::generateVariable("a"));
		entityManager.add(PkbEntity::generateVariable("b"));
		entityManager.add(PkbEntity::generateStatement("read x", 1));
		entityManager.add(PkbEntity::generateStatement("read y", 4));

		REQUIRE(entityManager.findStatement(lineNumber) == expectedResult);
	};

	SECTION("Valid line number") {
		testFindStatement(1, 3);
		testFindStatement(4, 4);
		testFindStatement(8, 0);
	}

	SECTION("Invalid line number") {
		testFindStatement(2, -1);
		testFindStatement(3, -1);
	}
}
