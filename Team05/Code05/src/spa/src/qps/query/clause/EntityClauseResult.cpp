#include "EntityClauseResult.h"

bool operator==(EntityClauseResult first, EntityClauseResult second) {
    return first.arg == second.arg && first.entities == second.entities;
}
