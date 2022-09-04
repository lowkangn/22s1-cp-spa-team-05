#pragma once

#include <string>
#include <vector>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

class PkbPattern {

public:
	PkbPattern(vector<Token> &lhsTokens, vector<Token> &rhsTokens) {
		this->lhsTokens = lhsTokens;
		this->rhsTokens = rhsTokens;
	}

private:
	vector<Token> lhsTokens;
	vector<Token> rhsTokens;
};