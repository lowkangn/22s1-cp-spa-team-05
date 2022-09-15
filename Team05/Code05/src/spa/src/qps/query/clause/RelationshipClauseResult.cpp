#include <qps/query/clause/RelationshipClauseResult.h>

bool operator==(RelationshipClauseResult first, RelationshipClauseResult second) {
    return first.firstArg == second.firstArg && first.secondArg == second.secondArg && first.relationships == second.relationships;
}
