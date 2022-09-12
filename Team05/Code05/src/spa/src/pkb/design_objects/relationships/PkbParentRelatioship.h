#pragma once
#include <pkb/design_objects/relationships/PkbRelationship.h>
#include <memory>
using namespace std;

class PkbParentRelationship : public PkbRelationship {
public:
	PkbParentRelationship(lhs shared_ptr<PkbEntity>, rhs shared_ptr<PkbEntity>) : PkbRelationship(lhs, rhs, PkbRelationshipType::PARENT)
};