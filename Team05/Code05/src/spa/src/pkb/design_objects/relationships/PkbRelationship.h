#pragma once
#include <pkb/design_objects/entities/PkbEntity.h>
#include <pkb/PkbException.h>

#include <memory>
#include <string>
using namespace std;

enum PkbRelationshipType {
	FOLLOWS,
	FOLLOWSSTAR,
	PARENT,
	PARENTSTAR,
	USES,
	MODIFIES
};

class PkbRelationship {
protected:
	shared_ptr<PkbEntity> lhs;
	shared_ptr<PkbEntity> rhs;
	PkbRelationshipType relationshipType;
	PkbRelationship(shared_ptr<PkbEntity> lhs, shared_ptr<PkbEntity> rhs, PkbRelationshipType relationshipType) {
		this->lhs = lhs;
		this->rhs = rhs;
		this->relationshipType = relationshipType;
	}

public: 
	shared_ptr<PkbEntity> getLhs() {
		return this->lhs;
	}

	shared_ptr<PkbEntity> getRhs() {
		return this->rhs;
	}

	bool isFollows() {
		return this->relationshipType == PkbRelationshipType::FOLLOWS;
	}
	bool isFollowsStar() {
		return this->relationshipType == PkbRelationshipType::FOLLOWSSTAR;
	}
	bool isParent() {
		return this->relationshipType == PkbRelationshipType::PARENT;
	}
	bool isParentStar() {
		return this->relationshipType == PkbRelationshipType::PARENTSTAR;
	}
	bool isUses() {
		return this->relationshipType == PkbRelationshipType::USES;
	}
	bool isModifies() {
		return this->relationshipType == PkbRelationshipType::MODIFIES;
	}

	string getKey() {
		// get string of relationship type

		string typeValue = to_string(this->relationshipType);

		// get key strings of lhs and rhs
		string lhsKey = this->lhs->getKey();
		string rhsKey = this->rhs->getKey();

		// create key from concatenating all 
		string key = lhsKey + rhsKey + typeValue;
		return key;
	}
};