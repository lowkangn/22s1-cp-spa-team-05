#include <pkb/pkbQueryManager/PkbQueryManager.h>





/*
	Converts an internal pkb pattern to a pql pattern used in the qps.
*/
PQLPattern PkbQueryManager::pkbPatternToPqlPattern(shared_ptr<PkbPattern> pattern) {
	string variableIdentifier = pattern->getVariableIdentifier();
	// TODO: this trim logic can be abstracted out
	// we store as space + variable + space, need to trim
	// left trim
	assert(variableIdentifier.length() > 0);
	bool leftIsSpace = isspace(variableIdentifier[0]);
	while (leftIsSpace && variableIdentifier.size() > 1) {

		variableIdentifier.erase(0, 1);
		leftIsSpace = isspace(variableIdentifier[0]);

	}

	// right trim
	bool rightIsSpace = isspace(variableIdentifier[variableIdentifier.size() - 1]);
	while (rightIsSpace && variableIdentifier.size() > 1) {
		variableIdentifier.erase(variableIdentifier.size() - 1, 1);
		rightIsSpace = isspace(variableIdentifier[variableIdentifier.size() - 1]);


	}
	return PQLPattern::generateAssignPattern(pattern->getStatementLineNumber(), variableIdentifier);
}

// ==================== Public methods ====================

// ******************** retrieval ********************
 
// -------------------- all --------------------
/*
	Retrieves all procedure entities.
*/
vector<PQLEntity> PkbQueryManager::retrieveAllProcedureEntities(shared_ptr<PkbRepository> repository) {
	// get
	vector<shared_ptr<PkbEntity>> all = this->entityHelper.retrieveAllProcedureEntities(repository);

	// do conversion	
	vector<PQLEntity> out;
	for (shared_ptr<PkbEntity> procedure : all) {

		// append to list
		out.push_back(this->pkbEntityToQpsPqlEntity(procedure));
	}
	return out;
}

/*
	Retrieves all statement entities, regardless of type.
*/
vector<PQLEntity> PkbQueryManager::retrieveAllStatementEntities(shared_ptr<PkbRepository> repository) {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->entityHelper.retrieveAllStatementEntities(repository);
	for (shared_ptr<PkbEntity> e : all) {

		// append to list
		out.push_back(this->pkbEntityToQpsPqlEntity(e));
	}
	return out;
}

/*
	Retrieves all variables.
*/
vector<PQLEntity> PkbQueryManager::retrieveAllVariables(shared_ptr<PkbRepository> repository) {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->entityHelper.retrieveAllVariables(repository);
	for (shared_ptr<PkbEntity> e : all) {

		// append to list
		out.push_back(this->pkbEntityToQpsPqlEntity(e));
	}
	return out;
}

/*
	Retrieves all constants.
*/
vector<PQLEntity> PkbQueryManager::retrieveAllConstants(shared_ptr<PkbRepository> repository) {
	vector<PQLEntity> out;
	vector<shared_ptr<PkbEntity>> all = this->entityHelper.retrieveAllConstants(repository);
	for (shared_ptr<PkbEntity> e : all) {

		// append to list
		out.push_back(this->pkbEntityToQpsPqlEntity(e));
	}
	return out;
}

// -------------------- by type --------------------
/*
	Retrieves all statement entities of a specified type.
*/
vector<PQLEntity> PkbQueryManager::retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType, shared_ptr<PkbRepository> repository) {

	// 0. get all statements
	vector<shared_ptr<PkbEntity>> statements = this->entityHelper.retrieveAllStatementEntities(repository);

	// 1. create a filter by type
	// we define it locally to keep it to the scope of this function
	typedef bool (*PkbStatementEntityFilter)(PkbStatementEntity* statement);

	PkbStatementEntityFilter filter = [](PkbStatementEntity* statement) {
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

	// 2. go through statements, filter, convert
	vector<PQLEntity> out;
	for (shared_ptr<PkbEntity> statement : statements) {
		// cast to statement entity
		assert(statement->isStatement());
		shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(statement);

		if (cast != nullptr && filter(cast.get())) { // use filter
			// create pql entity
			PQLEntity entity = this->pkbEntityToQpsPqlEntity(cast);
			out.push_back(entity);
		}
	}
	return out;

}

/*
	Retrieves all relationships of a specified type.
*/
vector<PQLRelationship> PkbQueryManager::retrieveRelationshipsByType(PKBTrackedRelationshipType relationshipType, shared_ptr<PkbRepository> repository) {
	// 1. see if need to get from table or not
	// also, map type to internal type
	PkbRelationshipType toGetType = this->pkbTrackedRelationshipTypeToInternalType(relationshipType);
	bool getFromTable = true;
	switch (relationshipType) {

	// graph types
	case PKBTrackedRelationshipType::AFFECTS:
	case PKBTrackedRelationshipType::AFFECTSSTAR:
		throw PkbException("Not implemented!");
	case PKBTrackedRelationshipType::NEXTSTAR:
		getFromTable = false;
		break;
	// table types 
	case PKBTrackedRelationshipType::CALLS:
	case PKBTrackedRelationshipType::CALLSSTAR:
	case PKBTrackedRelationshipType::CALLSTMTATTRIBUTE:
	case PKBTrackedRelationshipType::PARENT:
	case PKBTrackedRelationshipType::PARENTSTAR:
	case PKBTrackedRelationshipType::USES:
	case PKBTrackedRelationshipType::MODIFIES:
	case PKBTrackedRelationshipType::NEXT:
	case PKBTrackedRelationshipType::FOLLOWS:
	case PKBTrackedRelationshipType::FOLLOWSSTAR:
		getFromTable = true;
		break;
	}

	// 2a. get from table
	vector<shared_ptr<PkbRelationship>> all;
	if (getFromTable) {
		shared_ptr<PkbRelationshipTable> table = repository->getRelationshipTableByRelationshipType(toGetType);
		all = table->getAll();
	}
	else {
		// TODO: use graph extractors
		throw PkbException("Should be implemented, but isn't!");
	}
	
	// 2. convert all to PQLRelationship
	vector<PQLRelationship> out;
	for (shared_ptr<PkbRelationship> r : all) {

		PQLEntity lhs = this->pkbEntityToQpsPqlEntity(r->getLhs());
		PQLEntity rhs = this->pkbEntityToQpsPqlEntity(r->getRhs());
		out.push_back(PQLRelationship(lhs, rhs));
	}
	return out;
}

// -------------------- by identifier --------------------
/*
	Retrieves all statement entities by line number of a specified type.
*/
optional<PQLEntity> PkbQueryManager::retrieveStatementByLineNumberAndType(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType, shared_ptr<PkbRepository> repository) {
	// we know apriori that line number uniquely identifes
	// NOTE: currently, we keep all statements in a single table, as no two statements can have 
	// the same line number, and that serves as a good primary key.
	// by default, we just use a read statement as the entity to key on
	string key = PkbStatementEntity::createReadStatementEntity(lineNumber)->getKey();
	shared_ptr<PkbEntity> result = this->entityHelper.retrieveEntityByTypeAndKey(key);

	if (result == NULL) {
		return optional<PQLEntity>();
	}
	optional<PQLEntity> entity = this->pkbEntityToQpsPqlEntity(result);
	return entity;
}


/*
	Retrieves all procedure entities by name.
*/
optional<PQLEntity> PkbQueryManager::retrieveProcedureEntityByName(string procedureName, shared_ptr<PkbRepository> repository) {
	// create a procedure object out of it and use it to get a key
	string key = PkbProcedureEntity(procedureName).getKey();

	// get
	shared_ptr<PkbEntity> result = this->entityHelper.retrieveEntityByTypeAndKey(key);
	// if null, we return empty vector
	if (result == NULL) {
		return optional<PQLEntity>();
	}
	// else, we cast to PQLEntity
	optional<PQLEntity> entity = this->pkbEntityToQpsPqlEntity(result);
	return entity;
}


/*
	Retrieves all variables by a name.
*/
optional<PQLEntity> PkbQueryManager::retrieveVariableByName(string name, shared_ptr<PkbRepository> repository) {
	// create an object out of it and use it to get a key
	string key = PkbVariableEntity(name).getKey();

	// get
	shared_ptr<PkbEntity> result = this->entityHelper.retrieveEntityByTypeAndKey(key);
	// if null, we return empty vector
	if (result == NULL) {
		return optional<PQLEntity>();
	}
	// else, we cast to PQLEntity
	optional<PQLEntity> entity = this->pkbEntityToQpsPqlEntity(result);
	return entity;
}

/*
	Retrieves all constants by a value.
*/
optional<PQLEntity> PkbQueryManager::retrieveConstantByValue(int value, shared_ptr<PkbRepository> repository) {
	// create an object out of it and use it to get a key
	string key = PkbConstantEntity(value).getKey();

	// get
	repository->getEntityTableByType(PkbEntityType::CONSTANT).get(key);
	// if null, we return empty vector
	if (result == NULL) {
		return optional<PQLEntity>();
	}
	// else, we cast to PQLEntity
	optional<PQLEntity> entity = this->pkbEntityToQpsPqlEntity(result);
	return entity;
}

// -------------------- specifics --------------------

vector<PQLRelationship> PkbQueryManager::retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository) {

	// 1. translate the type
	PkbRelationshipType convertedRelationshipType = this->pkbTrackedRelationshipTypeToInternalType(relationshipType);

	// 2. depending on type, we pass to the correct handler
	vector<shared_ptr<PkbRelationship>> found;
	switch (convertedRelationshipType) {

		// graph types
	case PkbRelationshipType::NEXTSTAR:
	case PkbRelationshipType::AFFECTS:
	case PkbRelationshipType::AFFECTSSTAR:
		found = this->relationshipHelper.retrieveRelationshipsFromGraphsByTypeAndLhsRhs(convertedRelationshipType, lhs, rhs);
		break;

		// table types
	case PkbRelationshipType::NEXT:
	case PkbRelationshipType::FOLLOWS:
	case PkbRelationshipType::FOLLOWSSTAR:
	case PkbRelationshipType::MODIFIES:
	case PkbRelationshipType::PARENT:
	case PkbRelationshipType::PARENTSTAR:
	case PkbRelationshipType::USES:
	default:
		found = this->relationshipHelper.retrieveRelationshipsFromTablesByTypeAndLhsRhs(convertedRelationshipType, lhs, rhs);
	}

	// 3. then we convert to pql relationship
	vector<PQLRelationship> out;
	for (shared_ptr<PkbRelationship> r : found) {
		out.push_back(PQLRelationship(this->pkbEntityToQpsPqlEntity(r->getLhs()), this->pkbEntityToQpsPqlEntity(r->getRhs()));
	}
	return out;
}