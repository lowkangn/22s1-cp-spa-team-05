#include <qps/query/clause/PQLRelationship.h>

bool operator==(PQLRelationship first, PQLRelationship second) {
    return first.firstEntity == second.firstEntity && first.secondEntity == second.secondEntity;
}

bool operator<(PQLRelationship first, PQLRelationship second) {
    if (first.firstEntity == second.firstEntity) {
        return first.secondEntity < second.secondEntity;
    }
    return first.firstEntity < second.firstEntity;
}
