#include <qps/query/clause/ClauseArgument.h>


bool operator== (ClauseArgument first, ClauseArgument second) {
	return first.identifier == second.identifier
		&& first.type == second.type;
}