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

	PkbEntity readX = PkbEntity::generateStatement("read x", 1);
	PkbEntity varX = PkbEntity::generateVariable("x");
	PkbRelationship readModifiesX = PkbRelationship(entityManager.add(readX), entityManager.add(varX));
	PkbEntity readY = PkbEntity::generateStatement("read y", 2);
	PkbEntity varY = PkbEntity::generateVariable("y");
	PkbRelationship readModifiesY = PkbRelationship(entityManager.add(readY), entityManager.add(varY));
	PkbEntity assignX = PkbEntity::generateStatement("x = x + 1;", 3);
	PkbRelationship assignModifiesX = PkbRelationship(entityManager.add(assignX), entityManager.add(varX));
	PkbEntity assignY = PkbEntity::generateStatement("y = y + 2", 4);
	PkbRelationship assignModifiesY = PkbRelationship(entityManager.add(assignY), entityManager.add(varY));
	
	testAdd(entityManager, { readModifiesX, readModifiesY, assignModifiesX, assignModifiesY }, { 0, 1, 2, 3 });
}
