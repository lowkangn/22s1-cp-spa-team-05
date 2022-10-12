#pragma once
#include <pkb/design_objects/relationships/PkbRelationship.h>
#include <memory>
using namespace std;

class PkbNextRelationship : public PkbRelationship {
public:
	PkbNextRelationship(shared_ptr<PkbEntity> lhs, shared_ptr<PkbEntity> rhs) : PkbRelationship(lhs, rhs, PkbRelationshipType::NEXT) {}
};