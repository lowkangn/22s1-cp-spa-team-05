#include <qps/query/clause/PQLRelationship.h>

bool operator==(PQLRelationship first, PQLRelationship second) {
    return first.firstEntity == second.firstEntity && first.secondEntity == second.secondEntity;
}
