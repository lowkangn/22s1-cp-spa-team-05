#include <qps/query/clause/ModifiesSClause.h>

bool ModifiesSClause::equals (const Clause* other) {
	if (dynamic_cast<const ModifiesSClause*>(other) == nullptr) {
		return false;
	}
	ModifiesSClause otherModifies = *dynamic_cast<const ModifiesSClause*>(other);
	return lhs == otherModifies.lhs && rhs == otherModifies.rhs;
}