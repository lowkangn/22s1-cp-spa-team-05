#include "catch.hpp"

#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/PkbException.h>
#include <memory>
#include <string>
using namespace std;

TEST_CASE("PkbEntity: test ::getLineNumber") {
	auto test = [](shared_ptr<PkbEntity> entity, bool throws, int expectedLineNumber) {
		// given, when, then
		if (throws) {
			REQUIRE_THROWS_AS(entity->getLineNumber(), PkbException);
		}
		else {
			REQUIRE(entity->getLineNumber() == expectedLineNumber);
		}
	};

	SECTION("procedure entity throws") {
		string identifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbProcedureEntity(identifier));
		bool throws = true;
		int expectedLineNumber = -1;
		test(entity, throws, expectedLineNumber);
	};

	SECTION("variable entity throws") {
		string identifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbVariableEntity(identifier));
		bool throws = true;
		int expectedLineNumber = -1;
		test(entity, throws, expectedLineNumber);
	};

	SECTION("statement entity correct") {
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(1));
		bool throws = false;
		int expectedLineNumber = 1;
		test(entity, throws, expectedLineNumber);
	};
}

TEST_CASE("PkbEntity: test ::getIdentifier") {
	auto test = [](shared_ptr<PkbEntity> entity, bool throws, string expectedIdentifier) {
		// given, when, then
		if (throws) {
			REQUIRE_THROWS_AS(entity->getIdentifier(), PkbException);
		}
		else {
			REQUIRE(entity->getIdentifier() == expectedIdentifier);
		}
	};

	SECTION("procedure entity correct") {
		string expectedIdentifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbProcedureEntity(expectedIdentifier));
		bool throws = false;
		test(entity, throws, expectedIdentifier);
	};

	SECTION("variable entity correct") {
		string expectedIdentifier = "procedure";
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbVariableEntity(expectedIdentifier));
		bool throws = false;
		test(entity, throws, expectedIdentifier);
	};

	SECTION("statement entity throws") {
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(PkbStatementEntity::createReadStatementEntity(1));
		bool throws = true;
		string expectedIdentifier = "dummy";
		test(entity, throws, expectedIdentifier);
	};

}

	
