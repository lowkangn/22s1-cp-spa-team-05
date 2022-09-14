#pragma once
#include <pkb/design_objects/relationships/PkbRelationship.h>
#include <memory>
using namespace std;

class PkbFollowsRelationship : public PkbRelationship {
public:
	PkbFollowsRelationship(shared_ptr<PkbEntity> lhs, shared_ptr<PkbEntity> rhs) : PkbRelationship(lhs, rhs, PkbRelationshipType::FOLLOWS) {}
};