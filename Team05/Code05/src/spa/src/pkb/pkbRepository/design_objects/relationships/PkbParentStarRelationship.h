#pragma once
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>
#include <memory>
using namespace std;

class PkbParentStarRelationship : public PkbRelationship {
public:
	PkbParentStarRelationship(shared_ptr<PkbEntity> lhs, shared_ptr<PkbEntity> rhs) : PkbRelationship(lhs, rhs, PkbRelationshipType::PARENTSTAR) {}
};