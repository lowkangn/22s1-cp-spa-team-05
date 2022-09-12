#include "catch.hpp"

#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesStarRelationship.h>
#include <pkb/PkbException.h>

#include <memory>
#include <string>
using namespace std;

TEST_CASE("test PKBQueryHandlerbEntity::retrieveProcedureEntityByName") {
	
}

TEST_CASE("test PKBQueryHandlerbEntity::retrieveAllProcedureEntities") {

}