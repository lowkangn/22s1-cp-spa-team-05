#pragma once

#include <vector>

using namespace std;

class PkbRelationship {

public:
	PkbRelationship(int lhsId, int rhsId) {
		this->lhsId = lhsId;
		this->rhsId = rhsId;
	}

	int getLhs() {
		return this->lhsId;
	}

	int getRhs() {
		return this->rhsId;
	}
	

	bool equals(PkbRelationship other) {
		return this->lhsId == other.lhsId && this->rhsId == other.rhsId;
	}

private:
	int lhsId;
	int rhsId;
};
