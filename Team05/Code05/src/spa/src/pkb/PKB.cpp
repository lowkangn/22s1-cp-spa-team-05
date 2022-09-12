#include <pkb/PKB.h>
#include <pkb/PkbException.h>
#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>

vector<int> PKB::addEntities(vector<Entity> entities) {
	
	for (Entity entity : entities) {
		// depending on entity type, we create the appropriate pkb entity
		// and add to the appropriate table
		if (entity.isVariableEntity()) {

		}
		else if (
			entity.isAssignEntity()
			|| entity.isCallEntity()
			|| entity.isIf()
			|| entity.isPrintEntity()
			|| entity.isReadEntity()
			|| entity.isWhile()
			) {

		}
		else if (entity.isProcedureEntity()) {

		}
		else {
			throw PkbException("Unknown entity type being passed to PKB!")
		}

		
	}
	
}