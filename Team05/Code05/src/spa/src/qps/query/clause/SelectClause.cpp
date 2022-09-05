#include <qps/query/clause/SelectClause.h>

bool SelectClause::equals(Clause* other) {
	if (dynamic_cast<SelectClause*>(other) == nullptr) {
		return false;
	}
	SelectClause otherSelect = *dynamic_cast<SelectClause*>(other);
	return toSelect == otherSelect.toSelect;
}