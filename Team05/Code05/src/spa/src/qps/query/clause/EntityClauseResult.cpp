#include <qps/query/clause/EntityClauseResult.h>

bool operator==(EntityClauseResult first, EntityClauseResult second) {
	vector<vector<PQLEntity>> firstTable = first.table;
	vector<vector<PQLEntity>> secondTable = second.table;

	// sort the vectors so they can be element-wise compared
	sort(firstTable.begin(), firstTable.end());
	sort(secondTable.begin(), secondTable.end());

    return first.args[0] == second.args[0] && firstTable == secondTable;
}

bool operator<(EntityClauseResult first, EntityClauseResult second) {
	return first.args[0] < second.args[0] || (first.args[0] == second.args[0] && first.table < second.table);
}
