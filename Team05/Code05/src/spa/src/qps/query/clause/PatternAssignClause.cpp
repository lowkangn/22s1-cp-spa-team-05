#include <qps/query/clause/PatternAssignClause.h>

shared_ptr<RelationshipClauseResult> PatternAssignClause::execute(shared_ptr<PKBQueryHandler> pkb) {

	// With output, we really only care about the assign and the lhs (those are the only ones that vary and rhs is not
	// used in evaluation), so it can be represented in a PQLRelationship.
	vector<PQLRelationship> relationships;

	// LHS can be variable/wildcard or string literal
	// RHS can be wildcard, string literal or string with wildcards
	// Maybe we can have something like

	if (lhs.isStringLiteral()) {
		if (rhs.isWildcard()) {

		} else if (rhs.isStringLiteral()) {

		} else if (rhs.isStringWithWildcards()) {

		}
	} else if (lhs.isWildcard() || lhs.isVariableSynonym()) {
		if (rhs.isWildcard()) {

		} else if (rhs.isStringLiteral()) {

		} else if (rhs.isStringWithWildcards()) {

		}
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
