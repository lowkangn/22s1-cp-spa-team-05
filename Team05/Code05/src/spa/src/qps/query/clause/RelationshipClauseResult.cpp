#include <qps/query/clause/RelationshipClauseResult.h>

bool operator==(RelationshipClauseResult first, RelationshipClauseResult second) {
	Table firstTable = first.table;
	Table secondTable = second.table;

	// sort the vectors so they can be element-wise compared
	sort(firstTable.begin(), firstTable.end());
	sort(secondTable.begin(), secondTable.end());

	return first.getFirstArg() == second.getFirstArg() && first.getSecondArg() == second.getSecondArg() && firstTable == secondTable;
}
