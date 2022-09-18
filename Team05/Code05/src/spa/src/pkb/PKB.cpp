#include <pkb/PKB.h>
#include <pkb/PkbException.h>
#include <pkb/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/design_objects/entities/PkbVariableEntity.h>
#include <pkb/design_objects/entities/PkbConstantEntity.h>
#include <pkb/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/design_objects/relationships/PkbModifiesRelationship.h>
#include <qps/query/clause/PQLEntity.h>

#include <iostream>
using namespace std;

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
		// and case to pointer
		if (entity.isPrintEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createPrintStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isAssignEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createAssignStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isIf()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createIfStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isReadEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createReadStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isWhile()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createWhileStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isCallEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createCallStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else {
			throw PkbException("Unknown statement type being passed to PKB!");
		}
		
	}
	else if (entity.isProcedureEntity()) {
		// create pkb entity object
		string identifier = entity.getString();
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(new PkbProcedureEntity(identifier));
		return object;
	}
	else if (entity.isConstantEntity()) {
		int value = entity.getValue();
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(new PkbConstantEntity(value));
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
	} else {
		throw PkbException("Unknown PkbEntity type being passed to QPS!");
	}
}

shared_ptr<PkbRelationship> PKB::spRelationshipToPkbRelationship(Relationship relationship) {
	Entity lhs = relationship.getLhs();
	Entity rhs = relationship.getRhs();

	shared_ptr<PkbEntity> lhsToPkbEntity = this->spEntityToPkbEntity(lhs);
	shared_ptr<PkbEntity> rhsToPkbEntity = this->spEntityToPkbEntity(rhs);

	if (relationship.isFollows()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbFollowsRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isFollowsStar()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isModifies()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isParent()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbParentRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isParentStar()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbParentStarRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isUses()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbUsesRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else {
		throw PkbException("Unknown relationship being converted!");
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
		else if (entity.isConstantEntity()) {
			this->constantsTable.add(object);
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
		shared_ptr<PkbRelationship> pkbRelationship = spRelationshipToPkbRelationship(r);

		// depending on relationship type, we choose the table 
		// and create the object as a casted shared pointer
		
		if (r.isFollows()) {
			shared_ptr<PkbRelationshipTable> table = this->getFollowsTable();
			table->add(pkbRelationship);
		}
		else if (r.isFollowsStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getFollowsStarTable();
			table->add(pkbRelationship);
		}
		else if (r.isParent()) {
			shared_ptr<PkbRelationshipTable> table = this->getParentTable();
			table->add(pkbRelationship);
		}
		else if (r.isParentStar()) {
			shared_ptr<PkbRelationshipTable> table = this->getParentStarTable();
			table->add(pkbRelationship);
		}
		else if (r.isUses()) {
			shared_ptr<PkbRelationshipTable> table = this->getUsesTable();
			table->add(pkbRelationship);
		}
		else if (r.isModifies()) {
			shared_ptr<PkbRelationshipTable> table = this->getModifiesTable();
			table->add(pkbRelationship);
		}
		else {
			throw PkbException("Unknown relationship being added to PKB!");
		}

	}
}

void PKB::addPatterns(vector<Pattern> patterns) {
	// for every pattern
	for (Pattern p : patterns) {
		// only assign is supported
		if (!p.isAssignPattern()) { 
			throw PkbException("Only assign pattern is supported!");
		}
		// we get the strings
		vector<string> strings = {
			p.getLhs(),
			p.getRhs()
		};

		// we create the required pattern
		shared_ptr<PkbStatementPattern> pattern = PkbStatementPattern::createAssignPattern(p.getEntity().getLine(), strings);
		this->assignPatterns.add(pattern);
	}
}

PQLEntity PKB::retrieveProcedureEntityByName(string procedureName) {
	// create a procedure object out of it and use it to get a key
	string key = PkbProcedureEntity(procedureName).getKey();

	// get
	shared_ptr<PkbEntity> result = this->proceduresTable.get(key);
	// if null, we return null
	if (result == NULL) {
		throw PkbNotFoundException("Procedure not found!");
	}
	// else, we cast to PQLEntity
	return PQLEntity::generateProcedure(procedureName);

}

vector<PQLEntity> PKB::retrieveAllProcedureEntities() {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->proceduresTable.getAll();
	for (shared_ptr<PkbEntity> procedure : all) {

		// append to list
		out.push_back(PQLEntity::generateProcedure(procedure->getIdentifier()));
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
		string key = PkbStatementEntity::createReadStatementEntity(lineNumber)->getKey();
		shared_ptr<PkbEntity> result = this->statementTable.get(key);
		// if null, we return null
		if (result == NULL) {
			throw PkbNotFoundException("Statement not found!");
		}
		// else, we cast to PQLEntity
		return PQLEntity::generateStatement(lineNumber);
	}
	throw PkbException("Unknown statement type to be retrieved!");
}

vector<PQLEntity> PKB::retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) {
	vector<shared_ptr<PkbEntity>> result = this->statementTable.getAll();

	// initialize a default filter by type
	PkbStatementEntityFilter filter = [](PkbStatementEntity *statement) {
		return true;
	};

	// go through switch case to get specific filter by type
	if (pkbTrackedStatementType == PKBTrackedStatementType::ALL) {
		// do nothing
	}
	else if (pkbTrackedStatementType == PKBTrackedStatementType::READ) {
		filter = [](PkbStatementEntity* statement) {
			return statement->isReadStatement();
		};
	}
	else if (pkbTrackedStatementType == PKBTrackedStatementType::PRINT) {

		filter = [](PkbStatementEntity* statement) {
			return statement->isPrintStatement();
		};
	}
	else if (pkbTrackedStatementType == PKBTrackedStatementType::WHILE) {
		filter = [](PkbStatementEntity* statement) {
			return statement->isWhileStatement();
		};
	}
	else if (pkbTrackedStatementType == PKBTrackedStatementType::IF) {
		filter = [](PkbStatementEntity* statement) {
			return statement->isIfStatement();
		};
	}
	else if (pkbTrackedStatementType == PKBTrackedStatementType::ASSIGN) {
		filter = [](PkbStatementEntity* statement) {
			return statement->isAssignStatement();
		};
	}
	else if (pkbTrackedStatementType == PKBTrackedStatementType::CALL) {
		filter = [](PkbStatementEntity* statement) {
			return statement->isCallStatement();
		};
	}
	

	// go through statements in table and filter by type
	vector<PQLEntity> output;
	for (shared_ptr<PkbEntity> statement : this->statementTable.getAll()) {
		// cast to statement entity
		PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*statement));

		if (filter(cast)) { // use filter
			// create pql entity
			PQLEntity entity = PQLEntity::generateStatement(cast->getLineNumber());
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
		throw PkbNotFoundException("Variable not found!");
	}
	// else, we cast to PQLEntity
	return PQLEntity::generateVariable(name);
}

vector<PQLEntity> PKB::retrieveAllVariables() {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->variableTable.getAll();
	for (shared_ptr<PkbEntity> variable : all) {		
		// append to list
		out.push_back(PQLEntity::generateVariable(variable->getIdentifier()));
	}
	return out;
}

vector<PQLEntity> PKB::retrieveAllConstants() {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->constantsTable.getAll();
	for (shared_ptr<PkbEntity> variable : all) {
		// append to list
		out.push_back(PQLEntity::generateConstant(variable->getValue()));
	}
	return out;
}


shared_ptr<PkbRelationshipTable> PKB::getTableByRelationshipType(PKBTrackedRelationshipType relationshipType) {
	// based on relationship type, we get the specific table 
	switch (relationshipType) {
	case PKBTrackedRelationshipType::FOLLOWS:
		return this->getFollowsTable();
	case PKBTrackedRelationshipType::FOLLOWSSTAR:
		return this->getFollowsStarTable();
	case PKBTrackedRelationshipType::PARENT:
		return  this->getParentTable();
	case PKBTrackedRelationshipType::PARENTSTAR:
		return this->getParentStarTable();
	case PKBTrackedRelationshipType::USES:
		return this->getUsesTable();
	case PKBTrackedRelationshipType::MODIFIES:
		return this->getModifiesTable();
	default:
		throw PkbException("Unknown relationship type to be retrieved!");
	}
}



PkbEntityFilter getFilterFromClauseArgument(ClauseArgument arg, bool alwaysTrue) {

	// default filter is true
	PkbEntityFilter filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
		return true;
	};

	// depending on clause argument, we return the filter
	if (arg.isWildcard() || alwaysTrue) {
		return filter;
	}
	else if (arg.isVariableSynonym()) {
		// return only true if entity is a variable
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			return entity->isVariable();
		};
	}
	else if (arg.isProcedureSynonym()) {
		// return only true if entity is a procedure
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			return entity->isProcedure();
		};
	}
	else if (arg.isStmtSynonym()) {
		// return only true if entity is a statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			return entity->isStatement();
		};
	}
	else if (arg.isLineNumber()) {
		// entity must be a statement matching line number
		// return only true if entity has that line number
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if assign
			PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*entity));
			return cast->getLineNumber() == arg.getLineNumber();
		};
	}
	else if (arg.isStringLiteral()) {
		// can be procedure or variable name
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (entity->isVariable()) {
				// castand check 
				PkbVariableEntity* cast = dynamic_cast<PkbVariableEntity*>(&(*entity));
				return cast->getIdentifier() == arg.getIdentifier();
			}
			else if (entity->isProcedure()) {
				// castand check 
				PkbProcedureEntity* cast = dynamic_cast<PkbProcedureEntity*>(&(*entity));
				return cast->getIdentifier() == arg.getIdentifier();
			}
			return false;
		};

	}
	else if (arg.isAssignSynonym()) {
		// return only true if entity is a an assign statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if assign
			PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*entity));
			return cast->isAssignStatement();
		};
	}
	else if (arg.isPrintSynonym()) {
		// return only true if entity is a print statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if print
			PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*entity));
			return cast->isPrintStatement();
		};
	}
	else if (arg.isReadSynonym()) {
		// return only true if entity is a read statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if read
			PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*entity));
			return cast->isReadStatement();
		};
	}
	else if (arg.isCallSynonym()) {
		// return only true if entity is a call statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if call
			PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*entity));
			return cast->isCallStatement();
		};
	}
	else if (arg.isWhileSynonym()) {
		// return only true if entity is a while statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if while
			PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*entity));
			return cast->isWhileStatement();
		};
	}
	else if (arg.isIfSynonym()) {
		// return only true if entity is if  statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if if
			PkbStatementEntity* cast = dynamic_cast<PkbStatementEntity*>(&(*entity));
			return cast->isIfStatement();
		};
	}

	return filter;
}

vector<PQLRelationship> PKB::retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
	// 1. get table based on type
	shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(relationshipType);

	// 1.1. short circuiting
	// TODO this can be abstracted out into a vlidation function
	// 1.1.1 check that synonym does not refer to itself
	if (relationshipType == PKBTrackedRelationshipType::PARENT 
		|| relationshipType == PKBTrackedRelationshipType::PARENTSTAR
		|| relationshipType == PKBTrackedRelationshipType::FOLLOWS
		|| relationshipType == PKBTrackedRelationshipType::FOLLOWSSTAR) {
		if (lhs.isStmtSynonym() && rhs.isStmtSynonym() && lhs == rhs) {
			// is identical. e.g Follows(s,s)
			// no possible solution, return empty
			return vector<PQLRelationship>();
		}
	}


	// 2. if either side is exact, we can search by hash
	// we create the key we are looking for based on lhs and rhs 
	// TODO: for now, we do a manual filter

	// 3. if not, we have to manually filter
	PkbEntityFilter lhsFilter = getFilterFromClauseArgument(lhs);
	PkbEntityFilter rhsFilter = getFilterFromClauseArgument(rhs);
	vector<PQLRelationship> out;
	for (shared_ptr<PkbRelationship> r : table->getAll()) {
		shared_ptr<PkbEntity> lhsEntity = r->getLhs();
		shared_ptr<PkbEntity> rhsEntity = r->getRhs();

		
		if (lhsFilter(lhsEntity, lhs) && rhsFilter(rhsEntity, rhs)) {
			// create pql relationship
			PQLEntity outLhs = this->pkbEntityToQpsPqlEntity(lhsEntity);
			PQLEntity outRhs = this->pkbEntityToQpsPqlEntity(rhsEntity);
			out.push_back(PQLRelationship(outLhs, outRhs));
		}
	}
	return out;
}


vector<PQLRelationship> PKB::retrieveRelationshipsByType(PKBTrackedRelationshipType relationshipType) {
	// 1. get table based on type
	shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(relationshipType);

	// 2. convert all to PQLRelationship
	vector<PQLRelationship> out;
	for (shared_ptr<PkbRelationship> r : table->getAll()) {
		
		PQLEntity lhs = this->pkbEntityToQpsPqlEntity(r->getLhs());
		PQLEntity rhs = this->pkbEntityToQpsPqlEntity(r->getRhs());
		out.push_back(PQLRelationship(lhs, rhs));
	}
	return out;
}

shared_ptr<PKBQueryHandler> PKB::getQueryHandler() {
	return shared_ptr<PKBQueryHandler>(this);
}

shared_ptr<PKBUpdateHandler> PKB::getUpdateHandler() {
	return shared_ptr<PKBUpdateHandler>(this);
}

vector<PQLPattern> PKB::retrievePatterns(PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs) {
	
	if (statementType == PKBTrackedStatementType::ASSIGN) {
		return this->retrieveAssignPatterns(lhs, rhs);
		
	}
	else if (statementType == PKBTrackedStatementType::IF) {
		// TODO: we should move these out to an overloaded method that only takes in 1 clause argument
		throw PkbException("IF pattern type not implemented!");
	}
	else if (statementType == PKBTrackedStatementType::WHILE) {
		// TODO: we should move these out to an overloaded method that only takes in 1 clause argument
		throw PkbException("IF pattern type not implemented!");
	}
	else {
		throw PkbException("Unknown pattern type to be retrieved!");
	}
	
}


vector<PQLPattern> PKB::retrieveAssignPatterns(ClauseArgument lhs, ClauseArgument rhs) {
	// 1. parse lhs 
	string lhsStringPattern;

	bool lhsIsSynonym = false;
	if (lhs.isVariableSynonym() || lhs.isWildcard()) {
		// if lhs is a synonym, we treat as a wildcard, but flag it - we will deal with it later
		lhsIsSynonym = lhs.isVariableSynonym();
		// in either case, we match lhs with a wildcard
		lhsStringPattern = WILDCARD_CHAR;
	}
	else if (lhs.isStringLiteral() || lhs.isPatternString() || lhs.isPatternStringWithWildcards()) {
		// string literal, we match exactly
		lhsStringPattern = lhs.getIdentifier();
	}
	else {
		// should never get here
		throw PkbException("Unknown assign pattern being retrieved!");
	}

	// 2. parse rhs - it should only be wildcard or string literal
	string rhsStringPattern;
	if (rhs.isWildcard()) {
		rhsStringPattern = WILDCARD_CHAR;
	}
	else if (rhs.isPatternString() || rhs.isPatternStringWithWildcards()) {
		rhsStringPattern = rhs.getIdentifier();
	}
	else {
		throw PkbException("RHS of assign pattern should be wildcard, sandwiched wildcard or string, but is neither.");
	}

	// 3. find all patterns that match in the assign table
	vector<string> postFixStringsToMatch = {
		lhsStringPattern, rhsStringPattern
	};
	vector<shared_ptr<PkbStatementPattern>> matchingPatterns = this->assignPatterns.getAllThatMatchPostFixStrings(postFixStringsToMatch);


	// 5. for each returned statement, we get the corr. entity
	vector<PQLPattern> out;
	for (shared_ptr<PkbStatementPattern> p : matchingPatterns) {
		out.push_back(
			this->pkbPatternToPqlPattern(p)
		);
	}

	return out;
}

PQLPattern PKB::pkbPatternToPqlPattern(shared_ptr<PkbStatementPattern> p) {
	return PQLPattern::generateAssignPattern(p->getStatementLineNumber(), p->getVariableIdentifier());
}

bool PKB::containsEntity(Entity entity) {
	shared_ptr<PkbEntity> entitytoPkbEntity = spEntityToPkbEntity(entity);
	string key = entitytoPkbEntity->getKey();

	if (entitytoPkbEntity->isStatement()) {
		return this->statementTable.get(key) != NULL;
	}
	else if (entitytoPkbEntity->isVariable()) {
		return this->variableTable.get(key) != NULL;
	}
	else if (entitytoPkbEntity->isProcedure()) {
		return this->proceduresTable.get(key) != NULL;
	}
	else if (entitytoPkbEntity->isConstant()) {
		return this->constantsTable.get(key) != NULL;
	}
	else throw PkbException("Entity of unknown type being checked in PKB");
}

bool PKB::containsRelationship(Relationship relationship) {
	shared_ptr<PkbRelationship> relationshiptoPkbRelationship = spRelationshipToPkbRelationship(relationship);
	string key = relationshiptoPkbRelationship->getKey();

	if (relationshiptoPkbRelationship->isFollows()) {
		return this->getFollowsTable()->get(key) != NULL;
	}
	else if (relationshiptoPkbRelationship->isFollowsStar()) {
		return this->getFollowsStarTable()->get(key) != NULL;
	}
	else if (relationshiptoPkbRelationship->isParent()) {
		return this->getParentTable()->get(key) != NULL;
	}
	else if (relationshiptoPkbRelationship->isParentStar()) {
		return this->getParentStarTable()->get(key) != NULL;
	}
	else if (relationshiptoPkbRelationship->isUses()) {
		return this->getUsesTable()->get(key) != NULL;
	}
	else if (relationshiptoPkbRelationship->isModifies()) {
		return this->getModifiesTable()->get(key) != NULL;
	}
	else throw PkbException("Relationship of unknown type being checked in PKB");
}
