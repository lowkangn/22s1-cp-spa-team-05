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

	PkbWhilePattern(shared_ptr<PkbStatementEntity> statement, vector<string> strings) : PkbPattern(statement, strings) { };

public:
	bool isWhilePattern() {
		return this->statement->isWhileStatement();
	}

	bool isIfPattern() {
		return this->statement->isIfStatement();
	}

	bool isAssignPattern() {
		return this->statement->isAssignStatement();
	}

	/*
		Unlike for PKB statement pattern, we only consider LHS
	*/
	bool isRegexMatch(vector<string> regexStrings) {
		// Each pattern query for "while" can only be a single literal, a single synonym or a single wildcard
		if (regexStrings.size() != this->strings.size() || regexStrings.size() != 1) {
			throw PkbException("Trying to regex match, expecting exactly 1 regex string, just for lhs");
		}

		// Retrieves the string literal to match with the variable identifier 
		string regexString = regexStrings[0];

		// In while pattern, assert that there's only one var to match and retrieve that identifier
		assert(this->strings.size() == 1);
		string stringToMatch = this->strings[0];

		// Match the incoming string literal with the pattern's variable identifier
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

	/*
		Returns the identifying condition for the while statement
	*/
	string getVariableIdentifier() {
		assert(this->strings.size() == 1 && this->statement->isWhileStatement());
		return this->strings[0];
	}

	string getKey() {
		// we only expect one string for one while pattern entry
		assert(this->strings.size() == 1);
		string key = this->strings[0];
		return key + this->statement->getKey();
	}
};