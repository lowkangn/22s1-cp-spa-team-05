#pragma once
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>

#include <memory>
using namespace std;

class PkbAffectsRelationship : public PkbRelationship {
public:
    PkbAffectsRelationship(shared_ptr<PkbEntity> lhs, shared_ptr<PkbEntity> rhs) : PkbRelationship(
        lhs,
        rhs,
        PkbRelationshipType::AFFECTS) {
    }
};
