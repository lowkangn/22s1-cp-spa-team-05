#include <qps/query/clause/ModifiesSClause.h>

bool ModifiesSClause::equals (Clause* other) {
	if (dynamic_cast<ModifiesSClause*>(other) == nullptr) {
		return false;
	}
	ModifiesSClause otherModifies = *dynamic_cast<ModifiesSClause*>(other);
	return lhs == otherModifies.lhs && rhs == otherModifies.rhs;
}