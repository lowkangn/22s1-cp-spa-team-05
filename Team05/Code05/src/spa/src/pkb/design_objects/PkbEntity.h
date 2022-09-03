#pragma once

#include <string>
#include <vector>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

class PkbEntity {

public:
	PkbEntity(vector<Token> &tokens) {
		this->tokens = tokens;
	}

private:
	vector<Token> tokens;
};