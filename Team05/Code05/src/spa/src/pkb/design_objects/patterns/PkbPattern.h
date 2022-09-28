#pragma once
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/PkbException.h>

#include <vector>
#include <memory>
#include <regex>
#include <iostream>
using namespace std;

/*
	Parent class for all PKB patterns
*/

class PkbPattern {
protected:
	shared_ptr<PkbStatementEntity> statement;
	/*
		Assign pattern: expects 2 strings (lhs, rhs)
		If and while pattern: expects one string (lhs only)
	*/
	vector<string> strings;

public:
	PkbPattern(shared_ptr<PkbStatementEntity> statement, vector<string> strings) {
		this->strings = strings;
		this->statement = statement;
	}

	virtual bool isRegexMatch(vector<string> regexStrings) = 0;

	virtual string getKey() = 0;

	/*
		Ifs, whiles and assign statement entities have line numbers
	*/
	int getStatementLineNumber() {
		return this->statement->getLineNumber();
	}

	shared_ptr<PkbStatementEntity> getStatement() {
		return this->statement;
	}

	vector<string> getStringsToMatch() {
		return this->strings;
	}

	string getVariableIdentifier() {
		return this->strings[0];
	}

	bool equals(shared_ptr<PkbPattern> other) {
		return this->statement->equals(other->statement) && this->strings == other->strings;
	}
};