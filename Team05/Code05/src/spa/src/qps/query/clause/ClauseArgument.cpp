#include <qps/query/clause/ClauseArgument.h>

bool operator==(ClauseArgument first, ClauseArgument second) {
    return first.identifier == second.identifier && first.type == second.type;
}

bool operator!=(ClauseArgument first, ClauseArgument second) {
    return !(first == second);
}

bool operator<(ClauseArgument first, ClauseArgument second) {
    return first.type < second.type || (first.type == second.type && first.identifier < second.
        identifier);
}
