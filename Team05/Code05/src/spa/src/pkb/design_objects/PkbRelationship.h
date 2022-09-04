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

private:
	int lhsId;
	int rhsId;
};
