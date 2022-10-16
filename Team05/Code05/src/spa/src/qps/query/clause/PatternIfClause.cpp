#include <qps/query/clause/PatternIfClause.h>

shared_ptr<RelationshipClauseResult> PatternIfClause::execute(shared_ptr<PKBQueryHandler> pkb) {

	// check that rhs is wildcard
	assert(rhs.isWildcard());
	vector<PQLPattern> patterns;

	patterns = pkb->retrievePatterns(PKBTrackedStatementType::IF, lhs, rhs);

	// Convert to PQLRelationship and return RelationshipClauseResult
	vector<PQLRelationship> relationships;
	for (PQLPattern pattern : patterns) {
		PQLEntity stmtEntity = pattern.getStatementEntity();
		PQLEntity varEntity = pattern.getVariableEntity();
		relationships.emplace_back(stmtEntity, varEntity);
	}

	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(patternArg, lhs, relationships));
}

bool PatternIfClause::equals(shared_ptr<PatternClause> other) {
	if (dynamic_pointer_cast<PatternIfClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<PatternIfClause> otherPatternAssign = dynamic_pointer_cast<PatternIfClause>(other);
	return (this->patternArg == otherPatternAssign->patternArg) && (this->lhs == otherPatternAssign->lhs) && (this->rhs == otherPatternAssign->rhs);
}
