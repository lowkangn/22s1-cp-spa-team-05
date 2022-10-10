#include <qps/query/clause/RelationshipClauseResult.h>

bool operator==(RelationshipClauseResult first, RelationshipClauseResult second) {
	vector<vector<PQLEntity>> firstTable = first.table;
	vector<vector<PQLEntity>> secondTable = second.table;

	// sort the vectors so they can be element-wise compared
	sort(firstTable.begin(), firstTable.end());
	sort(secondTable.begin(), secondTable.end());

	return first.args[0] == second.args[0] && first.args[1] == second.args[1] && firstTable == secondTable;
}
