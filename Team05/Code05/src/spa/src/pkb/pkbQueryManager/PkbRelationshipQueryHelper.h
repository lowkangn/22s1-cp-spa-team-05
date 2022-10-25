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
#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbGraphNode.h>

#include <pkb/pkbQueryManager/graph_extractors/PkbGraphNextStarRelationshipExtractor.h>

#include <qps/query/clause/ClauseArgument.h>

#include <vector>


using namespace std;



class PkbRelationshipQueryHelper {
private:
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
PkbEntityFilter getFilterFromClauseArgument(ClauseArgument arg, bool alwaysTrue = false) {

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