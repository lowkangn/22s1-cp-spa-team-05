#include <qps/query/clause/PatternWhileClause.h>

PKBTrackedStatementType PatternWhileClause::getStatementType() {
	return PKBTrackedStatementType::WHILE;
}

bool PatternWhileClause::equals(shared_ptr<PatternClause> other) {
	if (dynamic_pointer_cast<PatternWhileClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<PatternWhileClause> otherPatternWhile = dynamic_pointer_cast<PatternWhileClause>(other);
	return (this->patternArg == otherPatternWhile->patternArg) && (this->lhs == otherPatternWhile->lhs) && (this->rhs == otherPatternWhile->rhs);
}