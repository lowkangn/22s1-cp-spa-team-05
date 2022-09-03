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

private:
	int lhsId;
	int rhsId;
};
