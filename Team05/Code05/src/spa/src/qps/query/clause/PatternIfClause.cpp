#include <qps/query/clause/PatternIfClause.h>

PKBTrackedStatementType PatternIfClause::getStatementType() {
	return PKBTrackedStatementType::IF;
}

bool PatternIfClause::equals(shared_ptr<PatternClause> other) {
	if (dynamic_pointer_cast<PatternIfClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<PatternIfClause> otherPatternAssign = dynamic_pointer_cast<PatternIfClause>(other);
	return (this->patternArg == otherPatternAssign->patternArg) && (this->lhs == otherPatternAssign->lhs) && (this->rhs == otherPatternAssign->rhs);
}
