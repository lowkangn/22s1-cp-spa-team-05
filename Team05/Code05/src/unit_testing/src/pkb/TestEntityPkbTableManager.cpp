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

	SECTION("Single add") {
		testAdd({ PkbEntity("a") }, { 0 });
		testAdd({ PkbEntity("read x", 1) }, { 0 });
	}

	SECTION("Multiple adds") {
		testAdd({ PkbEntity("a"), PkbEntity("b"), PkbEntity("c") }, { 0, 1, 2 });
		testAdd({ PkbEntity("read x", 1), PkbEntity("x") }, { 0, 1 });
		testAdd({ PkbEntity("read x", 1), PkbEntity("x = x + 1;", 2), PkbEntity("read y", 3) }, { 0, 1, 2 });
	}

	SECTION("Adding same entities") {
		testAdd({ PkbEntity("a"), PkbEntity("b"), PkbEntity("c"), PkbEntity("a"), PkbEntity("b") }, { 0, 1, 2, 0, 1 });
		testAdd({ PkbEntity("x"), PkbEntity("read x", 1), PkbEntity("read x", 1), PkbEntity("x") }, { 0, 1, 1, 0 });
	}

	SECTION("Adding similar statements with different line numbers") {
		testAdd({ PkbEntity("read x", 1), PkbEntity("read x", 2),  PkbEntity("read x", 3) }, { 0, 1, 2 });
	}
}

TEST_CASE("EntityPkbTableManager: test filter") {

	auto testFilter = [](vector<int> ids, vector<PkbEntity> expectedResult) {

		REQUIRE(ids.size() == expectedResult.size());

		EntityPkbTableManager entityManager = EntityPkbTableManager();

		entityManager.add(PkbEntity("x"));
		entityManager.add(PkbEntity("y"));
		entityManager.add(PkbEntity("a"));
		entityManager.add(PkbEntity("read x", 1));
		entityManager.add(PkbEntity("read y", 2));

		vector<PkbEntity> actualResults = entityManager.filter(ids);

		REQUIRE(actualResults.size() == expectedResult.size());

		for (int i = 0; i < actualResults.size(); i++) {
			actualResults[i] == expectedResult[i];
		}
	};

	testFilter({ 0, 1, 2, 3, 4 },
		{ PkbEntity("x"), PkbEntity("y"), PkbEntity("a"), PkbEntity("read x", 1), PkbEntity("read y", 2) });
	testFilter({ 4, 3, 1 }, { PkbEntity("read y", 2), PkbEntity("read x", 1), PkbEntity("y") });
	testFilter({ 1 }, { PkbEntity("y") });
}

TEST_CASE("EntityPkbTableManager: test clearDataBase") {

	auto testClear = []() {
		EntityPkbTableManager entityManager = EntityPkbTableManager();

		entityManager.add(PkbEntity("a"));
		entityManager.add(PkbEntity("b"));
		entityManager.add(PkbEntity("c"));
		entityManager.add(PkbEntity("read x", 1));
		entityManager.add(PkbEntity("read y", 2));

		entityManager.clearDataBase();

		REQUIRE(entityManager.add(PkbEntity("a")) == 0);
	};

	testClear();
}