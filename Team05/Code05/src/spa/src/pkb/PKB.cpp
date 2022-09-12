#include <pkb/PKB.h>
#include <pkb/PkbException.h>
#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesStarRelationship.h>
#include <pkb/design_objects/relationships/PkbModifiesRelationship.h>

shared_ptr<PkbEntity> PKB::spEntityToPkbEntity(Entity entity) {
	if (entity.isVariableEntity()) {

		// create pkb entity object
		string identifier = entity.getString();
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(new PkbVariableEntity(identifier));
		return object;
	}
	else if (entity.isStmtEntity()) {
		// read line number, since that's the statement identifier
		int lineNumber = entity.getLine();

		// depending on type, we call factory method
		if (entity.isPrintEntity()) {
			PkbStatementEntity object = PkbStatementEntity::createPrintStatementEntity(lineNumber);
		}
		else if (entity.isAssignEntity()) {
			PkbStatementEntity object = PkbStatementEntity::createAssignStatementEntity(lineNumber);
		}
		else if (entity.isIf()) {
			PkbStatementEntity object = PkbStatementEntity::createIfStatementEntity(lineNumber);
		}
		else if (entity.isReadEntity()) {
			PkbStatementEntity object = PkbStatementEntity::createReadStatementEntity(lineNumber);
		}
		else if (entity.isWhile()) {
			PkbStatementEntity object = PkbStatementEntity::createWhileStatementEntity(lineNumber);
		}
		else if (entity.isCallEntity()) {
			PkbStatementEntity object = PkbStatementEntity::createCallStatementEntity(lineNumber);
		}
		else {
			throw PkbException("Unknown statement type being passed to PKB!")
		}
		// create object and save into the table
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
		return object;
	}
	else if (entity.isProcedureEntity()) {
		// create pkb entity object
		string identifier = entity.getString();
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(new PkbProcedureEntity(identifier));
		return object;
	}
	else {
		throw PkbException("Unknown entity type being passed to PKB!");
	}
	
}


void PKB::addEntities(vector<Entity> entities) {
	
	for (Entity entity : entities) {
		// depending on entity type, we create the appropriate pkb entity
		// and add to the appropriate table
		shared_ptr<PkbEntity> object = this->spEntityToPkbEntity(entity);
		if (entity.isStmtEntity()) {
			this->statementTable.add(object);
		}
		else if (entity.isVariableEntity()) {
			this->variableTable.add(object);
		}
		else if (entity.isProcedureEntity()) {
			this->proceduresTable.add(object);
		}
		else {
			// do nothing for now
			// possible exception handling if needed
		}
	}
	
}

void PKB::addRelationships(vector<Relationship> relationships) {
	for (Relationship r : relationships) {
		// convert lhs and rhs to entities
		shared_ptr<PkbEntity> lhs = this->spEntityToPkbEntity(r.getLhs());
		shared_ptr<PkbEntity> rhs = this->spEntityToPkbEntity(r.getRhs());

		// depending on relationship type, we choose the table 
		// and create the object as a casted shared pointer
		if (r.isFollows()) {
			PkbRelationshipTable table = this->getFollowsTable();
			shared_ptr<PkbRelationship> relationship = shared_ptr<PkbRelationship>(new PkbFollowsRelationship(lhs, rhs));
			table.add(relationship);
		}
		else if (r.isFollowsStar()) {
			PkbRelationshipTable table = this->getFollowsStarTable();
			shared_ptr<PkbRelationship> relationship = shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(lhs, rhs));
			table.add(relationship);
		}
		else if (r.isParent()) {
			PkbRelationshipTable table = this->getParentTable();
			shared_ptr<PkbRelationship> relationship = shared_ptr<PkbRelationship>(new PkbParentRelationship(lhs, rhs));
			table.add(relationship);
		}
		else if (r.isParentStar()) {
			PkbRelationshipTable table = this->getParentStarTable();
			shared_ptr<PkbRelationship> relationship = shared_ptr<PkbRelationship>(new PkbParentStarRelationship(lhs, rhs));
			table.add(relationship);
		}
		else if (r.isUses()) {
			PkbRelationshipTable table = this->getUsesTable();
			shared_ptr<PkbRelationship> relationship = shared_ptr<PkbRelationship>(new PkbUsesRelationship(lhs, rhs));
			table.add(relationship);
		}
		else if (r.isModifies()) {
			PkbRelationshipTable table = this->getModifiesTable();
			shared_ptr<PkbRelationship> relationship = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhs, rhs));
			table.add(relationship);
		}
		else if (r.isUsesStar()) {
			PkbRelationshipTable table = this->getUsesStarTable();
			shared_ptr<PkbRelationship> relationship = shared_ptr<PkbRelationship>(new PkbUsesStarRelationship(lhs, rhs));
			table.add(relationship);
		}
		else {
			throw PkbException("Unknown relationship being added to PKB!")
		}

	}
}

void PKB::addPatterns(vector<Pattern> patterns) {
	// do nothing
}

