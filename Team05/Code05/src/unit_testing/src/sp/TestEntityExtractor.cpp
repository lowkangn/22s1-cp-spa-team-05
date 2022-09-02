#include "catch.hpp"
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/AST.h>

using namespace std;

TEST_CASE("EntityExtractor: test extractEntity") {


	auto testExtractEntity = [](ASTNode nodeToExtractFrom, Entity expectedEntity) {
		EntityExtractor* extractor = new EntityExtractor();

		Entity* extractedEntity = extractor->extractEntity(nodeToExtractFrom);

		REQUIRE(extractedEntity->equals(expectedEntity));
	};
}
