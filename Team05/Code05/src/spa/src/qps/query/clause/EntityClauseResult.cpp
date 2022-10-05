#include <qps/query/clause/EntityClauseResult.h>

bool operator==(EntityClauseResult first, EntityClauseResult second) {
	vector<PQLEntity> firstEntities = first.entities;
	vector<PQLEntity> secondEntities = second.entities;

	// sort the vectors so they can be element-wise compared
	sort(firstEntities.begin(), firstEntities.end());
	sort(secondEntities.begin(), secondEntities.end());

    return first.arg == second.arg && firstEntities == secondEntities;
}

bool operator<(EntityClauseResult first, EntityClauseResult second) {
	return first.arg < second.arg || (first.arg == second.arg && first.entities < second.entities);
}
