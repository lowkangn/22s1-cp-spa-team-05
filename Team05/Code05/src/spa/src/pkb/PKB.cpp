#include <pkb/PKB.h>
#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbConstantEntity.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbWhilePattern.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextRelationship.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <pkb/pkbRepository/graph_extractors/PkbGraphNextStarRelationshipExtractor.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallStmtAttributeRelationship.h>
#include <qps/query/clause/PQLEntity.h>
#include <StringSplitter.h>

#include <iostream>

using namespace std;


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

void PKB::addEntities(vector<Entity> entities) {
	
	this->updateManager.addEntities(entities, this->repository);
	
}

void PKB::addRelationships(vector<Relationship> relationships) {
	for (Relationship r : relationships) {
		// convert lhs and rhs to entities
		shared_ptr<PkbRelationship> pkbRelationship = externalRelationshipToPkbRelationship(r);

		// depending on relationship type, we choose the table 
		// and create the object as a casted shared pointer
		
		

	}
}

void PKB::addPatterns(vector<Pattern> patterns) {
	// for every pattern
	for (Pattern p : patterns) {
		int currLineNum = p.getEntity().getLine();
		if (p.isIfPattern()) {
			// get the pattern string from if pattern
			string conditions = p.getRhs();

			// split the pattern string and create a table (e.g. "x y z" -> [x, y, z])
			vector<string> variables = StringSplitter::splitStringByDelimiter(conditions, SPACE_DELIM);

			// create if pattern for each variable and add it to table
			for (string v : variables) {
				vector<string> varToAdd = { v };
				shared_ptr<PkbPattern> pattern = PkbIfPattern::createIfPattern(currLineNum, varToAdd);
				this->ifPatterns.add(pattern);
			}
		}
		else if (p.isWhilePattern()) {
			// get the pattern string from while pattern
			string conditions = p.getRhs();

			// split the pattern string and create a table (e.g. "x y z" -> [x, y, z])
			vector<string> variables = StringSplitter::splitStringByDelimiter(conditions, SPACE_DELIM);

			// create while pattern for each variable and add it to table
			for (string v : variables) {
				vector<string> varToAdd = { v };
				shared_ptr<PkbPattern> pattern = PkbWhilePattern::createWhilePattern(currLineNum, varToAdd);
				this->whilePatterns.add(pattern);
			}
		}
		else if (p.isAssignPattern()) {
			vector<string> strings = {
				p.getLhs(),
				p.getRhs()
			};
			
			// create required assign pattern
			shared_ptr<PkbPattern> pattern = PkbAssignPattern::createAssignPattern(currLineNum, strings);
			this->assignPatterns.add(pattern);

		}
		else {
			throw PkbException("Unknown pattern type being added!");
		}
	}
}





void PKB::addCfg(shared_ptr<CFGNode> rootNode) {

	


}

optional<PQLEntity> PKB::retrieveProcedureEntityByName(string procedureName) {
	// create a procedure object out of it and use it to get a key
	string key = PkbProcedureEntity(procedureName).getKey();

	// get
	shared_ptr<PkbEntity> result = this->proceduresTable.get(key);
	// if null, we return empty vector
	if (result == NULL) {
		return optional<PQLEntity>();
	}
	// else, we cast to PQLEntity
	optional<PQLEntity> entity = PQLEntity::generateProcedure(procedureName);
	return entity;
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

optional<PQLEntity> PKB::retrieveStatementByLineNumberAndType(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) {
	// NOTE: currently, we keep all statements in a single table, as no two statements can have 
	// the same line number, and that serves as a good primary key.
	// by default, we just use a read statement as the entity to key on
	string key = PkbStatementEntity::createReadStatementEntity(lineNumber)->getKey();
	shared_ptr<PkbEntity> result = this->statementTable.get(key);

	// filter the result by statement type and cast to PQLEntity
	vector<PQLEntity> entityVector = filterAndConvertStatementEntities({ result }, pkbTrackedStatementType);

	if (entityVector.empty()) {
		return optional<PQLEntity>();
	}
	optional<PQLEntity> entity = entityVector.front();
	return entity;
}

vector<PQLEntity> PKB::retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) {
	vector<shared_ptr<PkbEntity>> result = this->statementTable.getAll();
	return filterAndConvertStatementEntities(result, pkbTrackedStatementType);
}

vector<PQLEntity> PKB::retrieveAllStatementEntities() {
	return this->retrieveStatementEntitiesByType(PKBTrackedStatementType::ALL);
}

optional<PQLEntity> PKB::retrieveVariableByName(string name) {
	// create a procedure object out of it and use it to get a key
	string key = PkbVariableEntity(name).getKey();

	// get
	shared_ptr<PkbEntity> result = this->variableTable.get(key);
	// if null, we return empty optional
	if (result == NULL) {
		return optional<PQLEntity>();
	}
	// else, we cast to PQLEntity
	optional<PQLEntity> entity = PQLEntity::generateVariable(name);
	return entity;
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

optional<PQLEntity> PKB::retrieveConstantByValue(int value) {
	// create a procedure object out of it and use it to get a key
	string key = PkbConstantEntity(value).getKey();

	// get
	shared_ptr<PkbEntity> result = this->constantsTable.get(key);
	// if null, we return empty vector
	if (result == NULL) {
		return optional<PQLEntity>();
	}
	// else, we cast to PQLEntity
	optional<PQLEntity> entity = PQLEntity::generateConstant(value);
	return entity;
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
	case PKBTrackedRelationshipType::NEXT:
		return this->getNextTable();
	case PKBTrackedRelationshipType::CALLSTMTATTRIBUTE:
		return this->getCallsAttributeTable();
	case PKBTrackedRelationshipType::CALLS:
		return this->getCallsTable();
	case PKBTrackedRelationshipType::CALLSSTAR:
		return this->getCallsStarTable();
	default:
		throw PkbException("Unknown relationship type to be retrieved!");
	}
}

vector<PQLEntity> PKB::filterAndConvertStatementEntities(vector<shared_ptr<PkbEntity>> statements, 
	PKBTrackedStatementType pkbTrackedStatementType) {

	// initialize a default filter by type
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


	// go through statements in table and filter by type
	vector<PQLEntity> output;
	for (shared_ptr<PkbEntity> statement : statements) {
		// cast to statement entity
		shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(statement);

		if (cast != nullptr && filter(cast.get())) { // use filter
			// create pql entity
			PQLEntity entity = PQLEntity::generateStatement(cast->getLineNumber());
			output.push_back(entity);
		}
	}
	return output;
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


bool PKB::canShortCircuitRetrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {

	// 1. for parent, next, follows, cannot reference itself
	if (relationshipType == PKBTrackedRelationshipType::PARENT
		|| relationshipType == PKBTrackedRelationshipType::PARENTSTAR
		|| relationshipType == PKBTrackedRelationshipType::FOLLOWS
		|| relationshipType == PKBTrackedRelationshipType::FOLLOWSSTAR
		|| relationshipType == PKBTrackedRelationshipType::NEXT) {
		if (lhs.isStmtRefNoWildcard() && rhs.isStmtRefNoWildcard() && lhs == rhs) {
			// is identical. e.g Follows(s,s)
			// no possible solution, return empty
			return true;
		}
	} 

	return false;
}

shared_ptr<PkbEntity> PKB::convertClauseArgumentToPkbEntity(ClauseArgument clause) {
	// for now, we only support conversion of exact types
	if (!clause.isExactReference()) {
		throw PkbException("conversion only supports exact types (e.g. 1)!");
	}

	// we convert 
	if (clause.isLineNumber()) {
		shared_ptr<PkbEntity> entity = PkbStatementEntity::createPrintStatementEntity(clause.getLineNumber());
		return entity;
	}
	else if (clause.isStringLiteral()) {
		shared_ptr<PkbEntity> entity = shared_ptr<PkbEntity>(new PkbVariableEntity(clause.getIdentifier()));
		return entity;
	} else {
		throw PkbException("clause argument is of unknown type!");
	}

}


vector<PQLRelationship> PKB::retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
	// depending on type, we pass to the correct handler
	switch (relationshipType) {

	// graph types
	case PKBTrackedRelationshipType::NEXTSTAR:
	case PKBTrackedRelationshipType::AFFECTS:
	case PKBTrackedRelationshipType::AFFECTSSTAR:
		return retrieveRelationshipsFromGraphsByTypeAndLhsRhs(relationshipType, lhs, rhs);
		break;

	// table types
	case PKBTrackedRelationshipType::NEXT:
	case PKBTrackedRelationshipType::FOLLOWS:
	case PKBTrackedRelationshipType::FOLLOWSSTAR:
	case PKBTrackedRelationshipType::MODIFIES:
	case PKBTrackedRelationshipType::PARENT:
	case PKBTrackedRelationshipType::PARENTSTAR:
	case PKBTrackedRelationshipType::USES:
	default:
		return retrieveRelationshipsFromTablesByTypeAndLhsRhs(relationshipType, lhs, rhs);
	}
}

vector<PQLRelationship> PKB::retrieveRelationshipsFromGraphsByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {

	if (relationshipType == PKBTrackedRelationshipType::NEXTSTAR) {
		// 1. validation - must be a statement
		if (!lhs.isStmtRefNoWildcard() && !lhs.isWildcard()) {
			throw PkbException("NEXTSTAR relationship expects lhs and rhs to both be statements!");
		}

		// 2. there are four cases. 
		// if lhs and rhs are exact, we can do a direct check.
		// if exact and _, we need to do dfs from the specified node.
		// if _ and exact, we do dfs starting from the root node.
		// if _ and _, we do dfs from the root node and accumulate.
		vector<PQLRelationship> out;

		// case 1: both exact
		if (lhs.isExactReference() && rhs.isExactReference()) {
			// construct key from lhs and rhs
			shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
			shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);
			shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(lhsEntity);
			shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(rhsEntity);

			// create graph nodes and get their keys 
			string leftKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left)->getKey();
			string rightKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right)->getKey();

			// check they are both inside


			// if they are connected, return. else, return empty list
			if (this->cfgManager.isInside(leftKey) 
				&& this->cfgManager.isInside(rightKey)
				&& this->cfgManager.canReachNodeBFromNodeA(leftKey, rightKey)) {
				PQLEntity outLhs = this->pkbEntityToQpsPqlEntity(lhsEntity);
				PQLEntity outRhs = this->pkbEntityToQpsPqlEntity(rhsEntity);
				out.push_back(PQLRelationship(outLhs, outRhs));
			}
			return out;
		}


		// case 2: exact and wildcard
		PkbGraphNextStarRelationshipExtractor extractor;
		vector<shared_ptr<PkbRelationship>> extractedRelationships;
		if (lhs.isExactReference() && (rhs.isWildcard() || rhs.isSynonym())) {
			// convert lhs to entity, graph node, then get node 
			shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(this->convertClauseArgumentToPkbEntity(lhs));
			shared_ptr<PkbGraphNode> leftAsNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left);
			shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<PkbControlFlowGraphNode>(this->cfgManager.getNode(leftAsNode->getKey()));
			
			// starting from node, run dfs 
			extractor.extractAllFromStart(startNode, true);
			extractedRelationships = extractor.getExtractedRelationships();

		}
		// case 3: wildcard and exact
		else if ((lhs.isWildcard() || lhs.isSynonym()) && (rhs.isExactReference())) {
			// convert rhs to entity, graph node, then get target node 
			shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(this->convertClauseArgumentToPkbEntity(rhs));
			shared_ptr<PkbGraphNode> rightAsNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right);
			shared_ptr<PkbControlFlowGraphNode> endNode = static_pointer_cast<PkbControlFlowGraphNode>(this->cfgManager.getNode(rightAsNode->getKey()));
			shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<PkbControlFlowGraphNode>(this->cfgManager.getRootNode());

			// starting from node, run dfs 
			extractor.extractAllThatReachEnd(startNode, endNode, true);
			extractedRelationships = extractor.getExtractedRelationships();
		}
		else { // case 4: all wild card
			// starting at root node, dfs all the way
			shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<PkbControlFlowGraphNode>(this->cfgManager.getRootNode());
			extractor.extractAllFromStart(startNode);
			extractedRelationships = extractor.getExtractedRelationships();
		}

		// for each result in output, convert
		for (shared_ptr<PkbRelationship> r : extractedRelationships) {

			PQLEntity outLhs = this->pkbEntityToQpsPqlEntity(r->getLhs());
			PQLEntity outRhs = this->pkbEntityToQpsPqlEntity(r->getRhs());
			out.push_back(PQLRelationship(outLhs, outRhs));
		}
		return out;
	
	}
	else if (relationshipType == PKBTrackedRelationshipType::AFFECTS) {
		throw PkbException("Not implemented yet!");
	}
	else if (relationshipType == PKBTrackedRelationshipType::AFFECTSSTAR) {
		throw PkbException("Not implemented yet!");
	}
	else {
		throw PkbException("Unknown graph type relationship trying to be retrieved.");
	}
}


vector<PQLRelationship> PKB::retrieveRelationshipsFromTablesByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
	// 0. get table based on type
	shared_ptr<PkbRelationshipTable> table = this->getTableByRelationshipType(relationshipType);

	// 1. short circuiting
	if (canShortCircuitRetrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs)) {
		return vector<PQLRelationship>();
	}

	// 2. if both side are exact, we can search by hash
	// we create the key we are looking for based on lhs and rhs 
	if (lhs.isExactReference() && rhs.isExactReference()) {
		
		// 2.1 create relationship
		shared_ptr<PkbEntity> left = this->convertClauseArgumentToPkbEntity(lhs);
		shared_ptr<PkbEntity> right = this->convertClauseArgumentToPkbEntity(rhs);
		shared_ptr<PkbRelationship> toFind; // TODO: refactor to use factory methods
		switch (relationshipType) {
		case PKBTrackedRelationshipType::FOLLOWS:
			toFind = shared_ptr<PkbRelationship>(new PkbFollowsRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::FOLLOWSSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::PARENT:
			toFind = shared_ptr<PkbRelationship>(new PkbParentRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::PARENTSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbParentStarRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::USES:
			toFind = shared_ptr<PkbRelationship>(new PkbUsesRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::MODIFIES:
			toFind = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(left, right));
			break;
		case  PKBTrackedRelationshipType::NEXT:
			toFind = shared_ptr<PkbRelationship>(new PkbNextRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::CALLSTMTATTRIBUTE:
			toFind = shared_ptr<PkbRelationship>(new PkbCallStmtAttributeRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::CALLS:
			toFind = shared_ptr<PkbRelationship>(new PkbCallsRelationship(left, right));
			break;
		case PKBTrackedRelationshipType::CALLSSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbCallsStarRelationship(left, right));
			break;
		default:
			throw PkbException("Unknown relationship type to be retrieved!");
		}

		// 2.2 get by key
		string key = toFind->getKey();
		shared_ptr<PkbRelationship> found = table->get(key);


		// 2.3 if null, return empty 
		vector<PQLRelationship> retrievedRelationships;
		if (found == NULL) {
			return retrievedRelationships;
		}

		// 2.4 if not null, convert and return in single item vector
		PQLEntity outLhs = this->pkbEntityToQpsPqlEntity(found->getLhs());
		PQLEntity outRhs = this->pkbEntityToQpsPqlEntity(found->getRhs());
		retrievedRelationships.push_back(PQLRelationship(outLhs, outRhs));
		return retrievedRelationships;
	}
	

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
		assert(rhs.isWildcard());
		return this->retrieveIfPatterns(lhs);
	}
	else if (statementType == PKBTrackedStatementType::WHILE) {
		assert(rhs.isWildcard());
		return this->retrieveWhilePatterns(lhs);
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
	vector<shared_ptr<PkbPattern>> matchingPatterns = this->assignPatterns.getAllThatMatchPostFixStrings(postFixStringsToMatch);


	// 5. for each returned statement, we get the corr. entity
	vector<PQLPattern> out;
	for (shared_ptr<PkbPattern> p : matchingPatterns) {
		out.push_back(
			this->pkbPatternToPqlPattern(p)
		);
	}

	return out;
}

vector<PQLPattern> PKB::retrieveIfPatterns(ClauseArgument lhs) {
	// parse lhs 
	string lhsStringPattern;

	bool lhsIsSynonym = false;
	if (lhs.isVariableSynonym() || lhs.isWildcard()) {
		// if lhs is synonym, flag as wildcard
		lhsIsSynonym = lhs.isVariableSynonym();
		lhsStringPattern = WILDCARD_CHAR;
	}
	else if (lhs.isStringLiteral()) {
		// string literal, we match exactly
		lhsStringPattern = lhs.getIdentifier();
	}
	else {
		throw PkbException("Unknown if pattern being retrieved!");
	}


	vector<shared_ptr<PkbPattern>> matchingPatterns = this->ifPatterns.getVariableMatch(lhsStringPattern);

	vector<PQLPattern> out;
	for (shared_ptr<PkbPattern> p : matchingPatterns) {
		out.push_back(
			this->pkbPatternToPqlPattern(p)
		);
	}

	return out;
}

vector<PQLPattern> PKB::retrieveWhilePatterns(ClauseArgument lhs) {
	// parse lhs 
	string lhsStringPattern;

	bool lhsIsSynonym = false;
	if (lhs.isVariableSynonym() || lhs.isWildcard()) {
		// if lhs is synonym, flag as wildcard
		lhsIsSynonym = lhs.isVariableSynonym();
		lhsStringPattern = WILDCARD_CHAR;
	}
	else if (lhs.isStringLiteral()) {
		// string literal, we match exactly
		lhsStringPattern = lhs.getIdentifier();
	}
	else {
		throw PkbException("Unknown if pattern being retrieved!");
	}

	vector<shared_ptr<PkbPattern>> matchingPatterns = this->whilePatterns.getVariableMatch(lhsStringPattern);

	vector<PQLPattern> out;
	for (shared_ptr<PkbPattern> p : matchingPatterns) {
		out.push_back(
			this->pkbPatternToPqlPattern(p)
		);
	}

	return out;
}


PQLPattern PKB::pkbPatternToPqlPattern(shared_ptr<PkbPattern> p) {
	string variableIdentifier = p->getVariableIdentifier();
	// we store as space + variable + space, need to trim
	// left trim
	bool leftIsSpace = isspace(variableIdentifier[0]);
	while (leftIsSpace && variableIdentifier.size() > 1) {
		
		variableIdentifier.erase(0, 1);
		leftIsSpace = isspace(variableIdentifier[0]);
		
	}

	// right trim
	bool rightIsSpace = isspace(variableIdentifier[variableIdentifier.size()-1]);
	while (rightIsSpace && variableIdentifier.size() > 1) {
		variableIdentifier.erase(variableIdentifier.size() - 1, 1);
		rightIsSpace = isspace(variableIdentifier[variableIdentifier.size() - 1]);
		
		
	}
	return PQLPattern::generateAssignPattern(p->getStatementLineNumber(), variableIdentifier);
}

bool PKB::containsEntity(Entity entity) {
	shared_ptr<PkbEntity> entitytoPkbEntity = externalEntityToPkbEntity(entity);
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
	shared_ptr<PkbRelationship> relationshiptoPkbRelationship = externalRelationshipToPkbRelationship(relationship);
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
	else if (relationshiptoPkbRelationship->isCallStmtAttribute()) {
		return this->getCallsAttributeTable()->get(key) != NULL;
	}
	else if (relationshiptoPkbRelationship->isCalls()) {
		return this->getCallsTable()->get(key) != NULL;
	}
	else if (relationshiptoPkbRelationship->isCallsStar()) {
		return this->getCallsStarTable()->get(key) != NULL;
	}
	else throw PkbException("Relationship of unknown type being checked in PKB");
}
