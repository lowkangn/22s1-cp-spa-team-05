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
			|| relationshipType == PkbRelationshipType::CALLS
			|| relationshipType == PkbRelationshipType::CALLSSTAR 
			|| relationshipType == PkbRelationshipType::NEXT) {
			if (lhs.isStmtRefNoWildcard() && rhs.isStmtRefNoWildcard() && lhs == rhs) {
				// is identical. e.g Follows(s,s)
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