#include <qps/query/clause/PatternWhileClause.h>

shared_ptr<RelationshipClauseResult> PatternWhileClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	
	// check that rhs is wildcard
	assert(rhs.isWildcard());
	vector<PQLPattern> patterns;

	patterns = pkb->retrievePatterns(PKBTrackedStatementType::WHILE, lhs, rhs);

	// Convert to PQLRelationship and return RelationshipClauseResult
	vector<PQLRelationship> relationships;
	for (PQLPattern pattern : patterns) {
		PQLEntity stmtEntity = pattern.getStatementEntity();
		PQLEntity varEntity = pattern.getVariableEntity();
		relationships.emplace_back(stmtEntity, varEntity);
	}

	return shared_ptr<RelationshipClauseResult>(new RelationshipClauseResult(patternArg, lhs, relationships));
}

bool PatternWhileClause::equals(shared_ptr<PatternClause> other) {
	if (dynamic_pointer_cast<PatternWhileClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<PatternWhileClause> otherPatternWhile = dynamic_pointer_cast<PatternWhileClause>(other);
	return (this->patternArg == otherPatternWhile->patternArg) && (this->lhs == otherPatternWhile->lhs) && (this->rhs == otherPatternWhile->rhs);
}