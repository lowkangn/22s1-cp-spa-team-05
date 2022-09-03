#pragma once

#include <string>

using namespace std;

class PkbClause {

public:
	PkbClause(string lhs, string rhs) {
		this->lhs = lhs;
		this->rhs = rhs;
	}

	string getLhs() {
		return this->lhs;
	}

	string getRhs() {
		return this->getRhs;
	}

private:
	string lhs;
	string rhs;
};
