#pragma once
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>
#include <memory>
using namespace std;

class PkbAffectsStarRelationship : public PkbRelationship {
public:
	PkbAffectsStarRelationship(shared_ptr<PkbEntity> lhs, shared_ptr<PkbEntity> rhs) : PkbRelationship(lhs, rhs, PkbRelationshipType::	AFFECTSSTAR) {}
};