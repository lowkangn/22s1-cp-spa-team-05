#include <qps/query/clause/PatternAssignClause.h>

shared_ptr<RelationshipClauseResult> PatternAssignClause::execute(shared_ptr<PKBQueryHandler> pkb) {

	// With output, we really only care about the assign and the lhs (those are the only ones that vary and rhs is not
	// used in evaluation), so it can be represented in a PQLRelationship.
	vector<PQLPattern> patterns;

	patterns = pkb->retrievePatterns(PKBTrackedStatementType::ASSIGN, lhs, rhs);

	// for now we'll convert to PQLRelationship and return RelationshipClauseResult, may need to reevaluate in the future
	vector<PQLRelationship> relationships;
	for (PQLPattern pattern : patterns) {
		PQLEntity stmtEntity = pattern.getStatementEntity();
		PQLEntity varEntity = pattern.getVariableEntity();
		relationships.emplace_back(stmtEntity, varEntity);
	}

	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(patternArg, lhs, relationships));
}

bool PatternAssignClause::equals(shared_ptr<PatternClause> other) {
	if (dynamic_pointer_cast<PatternAssignClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<PatternAssignClause> otherPatternAssign = dynamic_pointer_cast<PatternAssignClause>(other);
	return (this->patternArg == otherPatternAssign->patternArg) && (this->lhs == otherPatternAssign->lhs) && (this->rhs == otherPatternAssign->rhs);
}
