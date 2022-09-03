#include "catch.hpp"
#include <pkb/design_objects/PkbEntity.h>
#include <pkb/table_managers/EntityPkbTableManager.h>
#include <pkb/table_managers/EntityPkbTableManager.cpp>

using namespace std;

TEST_CASE("PkbEntityTableManager: test add") {

	auto testAdd = [](PkbEntity entity, int expectedResult) {

		EntityPkbTableManager entityManager = EntityPkbTableManager();

		//REQUIRE(entityManager.add(entity) == expectedResult);
	};
}
