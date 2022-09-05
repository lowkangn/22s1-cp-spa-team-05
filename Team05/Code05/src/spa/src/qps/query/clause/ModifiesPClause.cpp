#include <qps/query/clause/ModifiesPClause.h>

bool ModifiesPClause::equals(const Clause* other) {
	if (dynamic_cast<const ModifiesPClause*>(other) == nullptr) {
		return false;
	}
	ModifiesPClause otherModifiesP = *dynamic_cast<const ModifiesPClause*>(other);
	return this->lhs == otherModifiesP.lhs && this->rhs == otherModifiesP.rhs;
}