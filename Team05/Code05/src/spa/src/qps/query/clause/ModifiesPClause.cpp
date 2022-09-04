#include <qps/query/clause/ModifiesPClause.h>

bool operator== (ModifiesPClause first, ModifiesPClause second) {
	return first.lhs == second.lhs && first.rhs == second.rhs;
};