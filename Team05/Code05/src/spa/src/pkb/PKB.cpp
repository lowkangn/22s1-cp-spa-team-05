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
#include <qps/query/clause/PQLEntity.h>

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

PQLEntity PKB::retrieveProcedureEntityByName(string procedureName) {
	// create a procedure object out of it and use it to get a key
	string key = PkbProcedureEntity(procedureName).getKey();

	// get
	shared_ptr<PkbEntity> result = this->proceduresTable.get(key);
	// if null, we return null
	if (result == NULL) {
		return result;
	}
	// else, we cast to PQLEntity
	return PQLEntity::generateProcedure(procedureName);

}

vector<PQLEntity> PKB::retrieveAllProcedureEntities() {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->proceduresTable.getAll();
	for (shared_ptr<PkbEntity> variable : all) {
		// cast to variable
		shared_ptr<PkbProcedureEntity> cast = dynamic_cast<shared_ptr<PkbProcedureEntity>>(variable);

		// append to list
		out.push_back(PQLEntity::generateVariable(cast->getIdentifier());
	}
	return out;
}

PQLEntity PKB::retrieveStatementEntityByLineNumber(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) {
	// create key
	switch (pkbTrackedStatementType) {
	case PKBTrackedStatementType::ALL:
	case PKBTrackedStatementType::READ:
	case PKBTrackedStatementType::PRINT:
	case PKBTrackedStatementType::WHILE:
	case PKBTrackedStatementType::IF:
	case PKBTrackedStatementType::ASSIGN:
	case PKBTrackedStatementType::CALL:
		// NOTE: currently, we keep all statements in a single table, as no two statements can have 
		// the same line number, and that serves as a good primary key.
		// by default, we just use a read statement as the entity to key on
		string key = PkbStatementEntity::createReadStatementEntity(lineNumber);
		shared_ptr<PkbEntity> result = this->statementTable.get(key);
		// if null, we return null
		if (result == NULL) {
			return result;
		}
		// else, we cast to PQLEntity
		return PQLEntity::generateStatement(lineNumber);
	default: 
		throw PkbException("Unknown statement type to be retrieved!");
	}
}

vector<PQLEntity> PKB::retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) {
	vector<shared_ptr<PkbEntity>> result = this->statementTable.getAll();
	switch (pkbTrackedStatementType) {
	case PKBTrackedStatementType::ALL:
		return result;
	case PKBTrackedStatementType::READ:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isReadStatement();
		}
		break;

	case PKBTrackedStatementType::PRINT:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isPrintStatement();
		}
		break;
	case PKBTrackedStatementType::WHILE:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isWhileStatement();
		}
		break;
	case PKBTrackedStatementType::IF:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isIfStatement();
		}
		break;
	case PKBTrackedStatementType::ASSIGN:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isAssignStatement();
		}
		break;
	case PKBTrackedStatementType::CALL:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isCallStatement();
		}
		break;
	default:
		throw PkbException("Unknown statement type to be retrieved (in bulk)!");
	}

	vector<PQLEntity> output;
	for (shared_ptr<PkbEntity> statement : this->statementTable) {
		// cast
		shared_ptr<PkbStatementEntity> casted = dynamic_cast<shared_ptr<PkbStatementEntity>>(statement);

		if (filter(casted)) {
			// create pql entity
			PQLEntity entity = PQLEntity::generateStatement(casted->getLineNumber());
			output.push_back(entity);
		}
	}
	return output;

}

vector<PQLEntity> PKB::retrieveAllStatementEntities() {
	return this->retrieveStatementEntitiesByType(PKBTrackedStatementType::ALL);
}

PQLEntity PKB::retrieveVariableByName(string name) {
	// create a procedure object out of it and use it to get a key
	string key = PkbVariableEntity(name).getKey();

	// get
	shared_ptr<PkbEntity> result = this->variableTable.get(key);
	// if null, we return null
	if (result == NULL) {
		return result;
	}
	// else, we cast to PQLEntity
	return PQLEntity::generateVariable(name);
}

vector<PQLEntity> PKB::retrieveAllVariables() {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->variableTable.getAll();
	for (shared_ptr<PkbEntity> variable : all) {
		// cast to variable
		shared_ptr<PkbVariableEntity> cast = dynamic_cast<shared_ptr<PkbVariableEntity>>(variable);
		
		// append to list
		out.push_back(PQLEntity::generateVariable(cast->getIdentifier());
	}
	return out;
}

shared_ptr<PKBQueryHandler> PKB::getQueryHandler() {
	return shared_ptr<PKBQueryHandler>(this);
}

shared_ptr<PKBUpdateHandler> PKB::getUpdateHandler() {
	return shared_ptr<PKBUpdateHandler>(this);
}