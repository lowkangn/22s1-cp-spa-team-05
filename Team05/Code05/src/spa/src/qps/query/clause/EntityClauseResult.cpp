#include <qps/query/clause/EntityClauseResult.h>

bool operator==(EntityClauseResult first, EntityClauseResult second) {
    Table firstTable = first.table;
    Table secondTable = second.table;

    // sort the vectors so they can be element-wise compared
    sort(firstTable.begin(), firstTable.end());
    sort(secondTable.begin(), secondTable.end());

    return first.getArg() == second.getArg() && firstTable == secondTable;
}

bool operator<(EntityClauseResult first, EntityClauseResult second) {
    return first.getArg() < second.getArg() || (first.getArg() == second.getArg() && first.table <
        second.table);
}
