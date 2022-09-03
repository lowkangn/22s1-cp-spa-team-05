#pragma once

#include <vector>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

class PkbRelationship {

public:
	PkbRelationship(int lhsId, int rhsId, vector<Tokens> &lhsTokens, vector<Token> rhsTokens) {
		this->lhsId = lhsId;
		this->rhsId = rhsId;
	}

private:
	int lhsId;
	int rhsId;
	vector<Token> lhsTokens;
	vector<Token> rhsTokens;
};
