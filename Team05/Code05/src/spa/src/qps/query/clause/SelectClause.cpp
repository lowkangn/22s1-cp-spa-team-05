#include <qps/query/clause/SelectClause.h>

bool operator== (SelectClause first, SelectClause second) {
	return first.toSelect == second.toSelect;
};