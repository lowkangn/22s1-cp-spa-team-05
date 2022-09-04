#include <qps/query/clause/ModifiesSClause.h>

bool operator== (ModifiesSClause first, ModifiesSClause second) {
	return first.lhs == second.lhs && first.rhs == second.rhs;
};