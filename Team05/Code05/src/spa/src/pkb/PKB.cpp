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

PQLEntity PKB::pkbEntityToQpsPqlEntity(shared_ptr<PkbEntity> entity) {
	// based on entity type, use pqlentity api
	if (entity->isProcedure()) {
		return PQLEntity::generateProcedure(entity->getIdentifier());
	}
	else if (entity->isVariable()) {
		return PQLEntity::generateVariable(entity->getIdentifier());
	}
	else if (entity->isStatement()) {
		return PQLEntity::generateStatement(entity->getLineNumber());
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

		// append to list
		out.push_back(PQLEntity::generateVariable(variable->getIdentifier());
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

	// initialize a default filter by type
	auto filter = [](shared_ptr<PkbStatementEntity> statement) {
		return true;
	};

	// go through switch case to get specific filter by type
	switch (pkbTrackedStatementType) {
	case PKBTrackedStatementType::ALL:
		return result;
	case PKBTrackedStatementType::READ:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isReadStatement();
		};
		break;

	case PKBTrackedStatementType::PRINT:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isPrintStatement();
		};
		break;
	case PKBTrackedStatementType::WHILE:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isWhileStatement();
		};
		break;
	case PKBTrackedStatementType::IF:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isIfStatement();
		};
		break;
	case PKBTrackedStatementType::ASSIGN:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isAssignStatement();
		};
		break;
	case PKBTrackedStatementType::CALL:
		auto filter = [](shared_ptr<PkbStatementEntity> statement) {
			return statement->isCallStatement();
		};
		break;
	default:
		throw PkbException("Unknown statement type to be retrieved (in bulk)!");
	}

	// go through statements in table and filter by type
	vector<PQLEntity> output;
	for (shared_ptr<PkbEntity> statement : this->statementTable) {
		// cast to statement entity
		shared_ptr<PkbStatementEntity> casted = dynamic_cast<shared_ptr<PkbStatementEntity>>(statement);

		if (filter(casted)) { // use filter
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
		// append to list
		out.push_back(PQLEntity::generateVariable(variable->getIdentifier());
	}
	return out;
}

PkbRelationshipTable PKB::getTableByRelationshipType(PKBTrackedRelationshipType relationshipType) {
	// based on relationship type, we get the specific table 
	switch (relationshipType) {
	case PKBTrackedRelationshipType::Follows:
		return this->getFollowsTable();
	case PKBTrackedRelationshipType::FollowsStar:
		return this->getFollowsStarTable();
	case PKBTrackedRelationshipType::Parent:
		return  this->getParentTable();
	case PKBTrackedRelationshipType::ParentStar:
		return this->getParentStarTable();
	case PKBTrackedRelationshipType::Uses:
		return this->getUsesTable();
	case PKBTrackedRelationshipType::UsesStar:
		throw PkbException("Not supported yet!");
	case PKBTrackedRelationshipType::Modifies:
		return this->getModifiesTable();
	case PKBTrackedRelationshipType::ModifiesStar:
		throw PkbException("Not supported yet!");
	default:
		throw PkbException("Unknown relationshp type to be retrieved!");
	}
}

vector<PQLRelationship> PKB::retrieveRelationshipsByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
	// 1. get table based on type
	PkbRelationshipTable table = this->getTableByRelationshipType(relationshipType);
	// 2. if either side is exact, we can search by hash
	// we create the key we are looking for based on lhs and rhs 
	// TODO: for now, we do a manual filter

	// 3. if not, we have to manually filter
	// 3.1 create lhs and rhs filters
	// if wildcard, always true
	// if statement variable, then check that is a statement
	// if variable variable, then check that is a variable
	// if procedure variable, then check that is 

	for (shared_ptr<PkbRelationship> r : table.getAll())) {
		
	}

}

PQLRelationship PKB::retrieveRelationshipByType(PKBTrackedRelationshipType relationshipType) {
	// 1. get table based on type
	PkbRelationshipTable table = this->getTableByRelationshipType(relationshipType);

	// 2. convert all to PQLRelationship
	vector<PQLRelationship> out;
	for (shared_ptr<PkbRelationship> r : table.getAll()) {
		
		PQLEntity lhs = this->pkbEntityToQpsPqlEntity(r->getLhs());
		PQLEntity rhs = this->pkbEntityToQpsPqlEntity(r->getRhs());
		out.push_back(PQLRelationship(lhs, rhs);
	}
	return out;
}

shared_ptr<PKBQueryHandler> PKB::getQueryHandler() {
	return shared_ptr<PKBQueryHandler>(this);
}

shared_ptr<PKBUpdateHandler> PKB::getUpdateHandler() {
	return shared_ptr<PKBUpdateHandler>(this);
}