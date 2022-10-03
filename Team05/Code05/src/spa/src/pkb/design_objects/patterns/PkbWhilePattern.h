#pragma once

#include <pkb/design_objects/patterns/PkbPattern.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/PkbException.h>

#include <vector>
#include <memory>
#include <regex>
#include <iostream>

class PkbWhilePattern : public PkbPattern {
protected:
	shared_ptr<PkbStatementEntity> statement;

	vector<string> strings;

	PkbWhilePattern(shared_ptr<PkbStatementEntity> statement, vector<string> strings) : PkbPattern(statement, strings) { };

public:
	bool isWhilePattern() {
		return getStatement()->isWhileStatement();
	}

	/*
		Unlike for PKB statement pattern, we only consider LHS
	*/
	bool isRegexMatch(vector<string> regexStrings) {
		if (regexStrings.size() != getStringsToMatch().size()) {
			throw PkbException("Trying to regex match, expecting exactly 1 regex string, just for lhs");
		}
		string regexString = regexStrings[0];
		string stringToMatch = getStringsToMatch()[0];
		if (!regex_match(stringToMatch, regex(regexString))) {
			return false;
		}
		else return true;
	}

	/*
		Factory method to create if pattern
	*/
	static shared_ptr<PkbWhilePattern> createWhilePattern(int lineNumber, vector<string> strings) {
		if (strings.size() != 1) {
			throw PkbException("Trying to create while pattern, expecting only 1 string.");
		}
		shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createWhileStatementEntity(lineNumber);
		return shared_ptr<PkbWhilePattern>(new PkbWhilePattern(statement, strings));
	}

	string getKey() {
		string key = getStringsToMatch()[0]; // we only expect one pattern to map to one while statement
		return key + getStatement()->getKey();
	}
};