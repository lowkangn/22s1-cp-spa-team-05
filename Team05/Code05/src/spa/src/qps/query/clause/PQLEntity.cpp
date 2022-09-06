#include "PQLEntity.h"

bool operator==(PQLEntity first, PQLEntity second) {
    return first.type == second.type && first.str == second.str && first.lineNum == second.lineNum && first.value == second.value;
}
