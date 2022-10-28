#include <qps/query/clause/PatternClause.h>

shared_ptr<RelationshipClauseResult> PatternClause::execute(shared_ptr<PKBQueryHandler> pkb) {

	// With output, we really only care about the assign and the lhs (those are the only ones that vary and rhs is not
	// used in evaluation), so it can be represented in a PQLRelationship.
	vector<PQLPattern> patterns;

	patterns = pkb->retrievePatterns(this->getStatementType(), lhs, rhs);

	// for now we'll convert to PQLRelationship and return RelationshipClauseResult, may need to reevaluate in the future
	vector<PQLRelationship> relationships;
	for (PQLPattern pattern : patterns) {
		PQLEntity stmtEntity = pattern.getStatementEntity();
		PQLEntity varEntity = pattern.getVariableEntity();
		relationships.emplace_back(stmtEntity, varEntity);
	}

	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(patternArg, lhs, relationships));
}