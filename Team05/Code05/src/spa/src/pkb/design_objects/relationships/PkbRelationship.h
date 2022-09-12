#pragma once
#include <pkb/design_objects/entities/PkbEntity.h>
#include <pkb/PkbException.h>

#include <memory>
#include <string>
using namespace std;

enum class PkbRelationshipType {
	FOLLOWS,
	FOLLOWSSTAR,
	PARENT,
	PARENTSTAR,
	USES,
	USESSTAR,
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
	bool isUsesStar() {
		return this->relationshipType == PkbRelationshipType::USESSTAR;
	}
	bool isModifies() {
		return this->relationshipType == PkbRelationshipType::MODIFIES;
	}

	string getKey() {
		// get string of relationshiph type
		string typeValue;
		if (this->isFollows()) {
			typeValue = "0";
		}
		else if (this->isFollowsStar()) {
			typeValue = "1";
		}
		else if (this->isParent()) {
			typeValue = "2";
		}
		else if (this->isParentStar()) {
			typeValue = "3";
		}
		else if (this->isUses()) {
			typeValue = "4";
		}
		else if (this->isModifies()) {
			typeValue = "5";
		}
		else {
			throw PkbException("Unknown relationship type to be hashed.");
		}

		// get key strings of lhs and rhs
		string lhsKey = this->lhs->getKey();
		string rhsKey = this->rhs->getKey();

		// create key from concatenating all 
		string key = lhsKey + rhsKey + typeValue;
		return key;
	}
};
