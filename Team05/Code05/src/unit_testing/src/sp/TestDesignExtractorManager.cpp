#include "catch.hpp"

#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/DesignExtractorManager.cpp>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/AST.h>

TEST_CASE("DesignExtractor: test : extractEntity()") {
	auto testExtractAll = [](shared_ptr<ASTNode> nodeToExtractFrom, vector<Entity> expectedEntity) {

		EntityExtractor extractor = EntityExtractor();
		EntityExtractor extractor = EntityExtractor();
		ModifiesExtractor extractor = EntityExtractor();

		EntityExtractor extractor = DesignExtractorManager();

		Entity extractedEntity = extractor.extractEntity(nodeToExtractFrom);

		REQUIRE(extractedEntity.equals(expectedEntity));
	};
}