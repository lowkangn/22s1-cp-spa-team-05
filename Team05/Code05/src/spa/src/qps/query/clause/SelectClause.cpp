#include <qps/query/clause/SelectClause.h>

bool SelectClause::equals(const Clause* other) {
	if (dynamic_cast<const SelectClause*>(other) == nullptr) {
		return false;
	}
	SelectClause otherSelect = *dynamic_cast<const SelectClause*>(other);
	return toSelect == otherSelect.toSelect;
}