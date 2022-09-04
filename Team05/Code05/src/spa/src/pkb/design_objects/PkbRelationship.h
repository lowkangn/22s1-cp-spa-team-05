#pragma once

#include <vector>
#include <sp/dataclasses/tokens/Token.h>

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
