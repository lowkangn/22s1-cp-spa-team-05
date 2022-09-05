#include <qps/query/clause/ModifiesPClause.h>

bool ModifiesPClause::equals(Clause* other) {
	if (dynamic_cast<ModifiesPClause*>(other) == nullptr) {
		return false;
	}
	ModifiesPClause otherModifiesP = *dynamic_cast<ModifiesPClause*>(other);
	return lhs == otherModifiesP.lhs && rhs == otherModifiesP.rhs;
}