#pragma once
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>
#include <memory>
using namespace std;

class PkbNextStarRelationship : public PkbRelationship {
public:
	PkbNextStarRelationship(shared_ptr<PkbEntity> lhs, shared_ptr<PkbEntity> rhs) : PkbRelationship(lhs, rhs, PkbRelationshipType::NEXTSTAR) {}
};