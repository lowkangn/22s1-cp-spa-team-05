#pragma once

#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbConstantEntity.h>
#include <pkb/pkbRepository/PkbRepository.h>

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
#include <pkb/pkbRepository/design_objects/relationships/PkbNextStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNotNextStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallStmtAttributeRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbAffectsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbAffectsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNotAffectsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNotAffectsRelationship.h>


#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbGraphNode.h>

#include <pkb/pkbQueryManager/graph_extractors/PkbGraphNextStarRelationshipExtractor.h>
#include <pkb/pkbQueryManager/graph_extractors/PkbGraphAffectsRelationshipExtractor.h>


#include <qps/query/clause/ClauseArgument.h>

#include <optional>
#include <vector>


using namespace std;



class PkbRelationshipQueryHelper {
private:
	// ******************** states ********************
	/*
		Maintains a set of edges explored for the affects* helper method, which 
		needs to traverse an affects graph.
	*/
	struct EdgeKeyHash {
		size_t operator()(const pair<string, string>& p) const {
			// we choose 31 as it's a prime number typically used for hashing strings
			return hash<string>()(p.first) * 31 + hash<string>()(p.second);

		}
	};
	unordered_set<pair<string, string>, EdgeKeyHash> visitedAffectsEdges;

	// ******************** helper methods ********************
	/*
		Helper function to check if retrieving the relationship, while semantically and syntacticall correct, is even
		possible. e.g. Follows(s,s) is not possible
	*/
	bool canShortCircuitRetrieveRelationshipByTypeAndLhsRhs(PkbRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) {
		// 1. for parent, next, follows, cannot reference itself
		if (relationshipType == PkbRelationshipType::PARENT
			|| relationshipType == PkbRelationshipType::PARENTSTAR
			|| relationshipType == PkbRelationshipType::FOLLOWS
			|| relationshipType == PkbRelationshipType::FOLLOWSSTAR
			|| relationshipType == PkbRelationshipType::NEXT) {
			if (lhs.isStmtRefNoWildcard() && rhs.isStmtRefNoWildcard() && lhs == rhs) {
				// is identical. e.g Follows(s,s)
				// no possible solution, return empty
				return true;
			}
		} else if (relationshipType == PkbRelationshipType::CALLS
			|| relationshipType == PkbRelationshipType::CALLSSTAR) {
            if (lhs.isProcedureSynonym() && rhs.isProcedureSynonym() && lhs == rhs) {
                // is identical. e.g Calls(p,p)
                // no possible solution, return empty
                return true;
            }
		}

		return false;
	}


	/*
		Helper function to convert a clause argument into a pkb entity it's trying to specify.
	*/
	shared_ptr<PkbEntity> convertClauseArgumentToPkbEntity(ClauseArgument clause) {
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
		}
		else {
			throw PkbException("clause argument is of unknown type!");
		}
	}

	/*
		Given a relationship, checks if there is a match.
	*/
	shared_ptr<PkbRelationship> findPkbRelationshipFromRepository(shared_ptr<PkbRelationship> relationship, shared_ptr<PkbRepository> repository) {
		// 1. key 
		string key = relationship->getKey();

		// 2. query the table
		shared_ptr<PkbRelationship> found = repository->getRelationshipTableByRelationshipType(relationship->getType())->get(key);
		return found;
	}

	/*
		Given an entity, checks if there is a match.
	*/
	shared_ptr<PkbEntity> findEntityFromRepository(shared_ptr<PkbEntity> entity, shared_ptr<PkbRepository> repository) {
		// 1. key 
		string key = entity->getKey();

		// 2. query the table
		shared_ptr<PkbEntity> found = repository->getEntityTableByType(entity->getType())->get(key);
		return found;
	}

	/*
		Given a relationship, adds it to the repository.
	*/
	void addPkbRelationship(shared_ptr<PkbRelationship> relationship, shared_ptr<PkbRepository> repository) {
		repository->getRelationshipTableByRelationshipType(relationship->getType())->add(relationship);
	}

	// ******************** relationship query handlers ********************
	/*
		Handles the (complicated) logic of retrieval for next* relationships.
	*/
	vector<shared_ptr<PkbRelationship>> retrieveNextStarByTypeAndLhsRhs(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);

	/*
		Handles the (complicated) logic of retrieval for affects relationships.
	*/
	vector<shared_ptr<PkbRelationship>> retrieveAffectsByTypeAndLhsRhs(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);

	/*
		Handles the (complicated) logic of retrieval for affects* relationships.
	*/
	vector<shared_ptr<PkbRelationship>> retrieveAffectsStarByTypeAndLhsRhs(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);

	// ******************** relationship query helpers ********************
	// -------------------- affects & affects* --------------------
	/*
		Checks that an entity is assign entity and exists. This is necessary because 
		affects and affects* are defined by assign on both sides.
	*/
	bool entityExistAndIsAssign(shared_ptr<PkbEntity> entity, shared_ptr<PkbRepository> repository) {
		shared_ptr<PkbEntity> found = this->findEntityFromRepository(entity, repository);
		// if both don't exist, false
		if (found == NULL) {
			return false;
		}
		else {
			// cast and check if assign
			shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(found);
			assert(cast != nullptr);
			if (!cast->isAssignStatement()) {
				return false;
			}
		}
		return true;
	}

	/*
		Gets the intersecting string key -> variable entity map between a lhs that modifies v
		and a rhs that uses v. Is intended for assign(exact, exact).
	*/
	unordered_map<string, shared_ptr<PkbEntity>> getLhsModifiesRhsUsesIntersectingVariableMap(ClauseArgument lhs, ClauseArgument rhs, shared_ptr <PkbRepository> repository) {
		// 2.1 find all v lhs modifies and convert to set
		ClauseArgument variable = ClauseArgument::createVariableArg("v");
		vector<shared_ptr<PkbRelationship>> lhsModifies = this->retrieveRelationshipsFromTablesByTypeAndLhsRhs(PkbRelationshipType::MODIFIES, lhs, variable, repository);
		unordered_map<string, shared_ptr<PkbEntity>> lhsModifiesMap;
		for (shared_ptr<PkbRelationship> relationship : lhsModifies) {
			lhsModifiesMap.insert({ relationship->getRhs()->getKey(), relationship->getRhs() });
		}

		// 2.2 find all v rhs uses and convert to set
		vector<shared_ptr<PkbRelationship>> rhsUses = this->retrieveRelationshipsFromTablesByTypeAndLhsRhs(PkbRelationshipType::USES, rhs, variable, repository);
		unordered_map<string, shared_ptr<PkbEntity>> rhsUsesMap;
		for (shared_ptr<PkbRelationship> relationship : rhsUses) {
			rhsUsesMap.insert({ relationship->getRhs()->getKey(), relationship->getRhs() });
		}

		// 2.3 get intersection and see if size is 0
		unordered_map<string, shared_ptr<PkbEntity>> intersectingVariableMap;
		for (auto keyValue = rhsUsesMap.begin(); keyValue != rhsUsesMap.end(); keyValue++) {
			string key = keyValue->first;
			shared_ptr<PkbEntity> variable = keyValue->second;
			if (lhsModifiesMap.find(key) != lhsModifiesMap.end()) {
				intersectingVariableMap.insert({ variable->getKey(), variable });
			}
		}
		return intersectingVariableMap;
	}

public:
	
	/*
		Retrieves all relationships by a lhs, rhs for relationships of a specified type, from tables.
	*/
	vector<shared_ptr<PkbRelationship>> retrieveRelationshipsFromTablesByTypeAndLhsRhs(PkbRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);

	/*
		Retrieves all relationships by a lhs, rhs for relationships of a specified type, from graphs.
	*/
	vector<shared_ptr<PkbRelationship>> retrieveRelationshipsFromGraphsByTypeAndLhsRhs(PkbRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);
};


/*
	A filter applied to a pkb entity. This is used
	in filtering relationships by lhs and rhs.
*/
typedef bool (*PkbEntityFilter)(shared_ptr<PkbEntity> entity, ClauseArgument arg);

/*
	Converts a clause argument to a filter that can be applied to a PkbEntity. This is used
	in filtering relationships by lhs and rhs. We provide a flag to return a dummy filter
	that always evaluates to true for ease.
*/
PkbEntityFilter getFilterFromClauseArgument(ClauseArgument arg, bool alwaysTrue = false);