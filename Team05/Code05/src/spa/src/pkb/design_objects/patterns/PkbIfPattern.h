#pragma once

#include <pkb/design_objects/patterns/PkbPattern.h>
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/PkbException.h>

#include <vector>
#include <memory>
#include <regex>
#include <iostream>
#include <assert.h>


class PkbIfPattern : public PkbPattern {
protected:

	PkbIfPattern(shared_ptr<PkbStatementEntity> statement, vector<string> strings) : PkbPattern(statement, strings) { };

public:
	bool isIfPattern() {
		return this->statement->isIfStatement();
	}

	/*
		Unlike for PKB statement pattern, we only consider LHS
	*/
	bool isRegexMatch(vector<string> regexStrings) {
		// Each pattern query for "if" can only be a single literal, a single synonym or a single wildcard
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
	static shared_ptr<PkbIfPattern> createIfPattern(int lineNumber, vector<string> strings) {
		if (strings.size() != 1) {
			throw PkbException("Trying to create if pattern, expecting only 1 string.");
		}
		shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createIfStatementEntity(lineNumber);
		return shared_ptr<PkbIfPattern>(new PkbIfPattern(statement, strings));
	}

	string getKey() {
		// we only expect one string for one if pattern entry
		assert(this->strings.size() == 1);
		string key = this->strings[0]; 
		return key + this->statement->getKey();
	}
};