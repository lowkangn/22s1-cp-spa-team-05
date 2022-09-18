#pragma once
#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/PkbException.h>

#include <vector>
#include <memory>
#include <regex>
#include <iostream>
using namespace std;
/*
	Parent class for all statement-based patterns.
	If the need ever arises for other pattern types, we can refactor 
	upwards (add a parent class). 
*/
class PkbStatementPattern {
protected:
	/*
		Entity in question.
	*/
	shared_ptr<PkbStatementEntity> statement;
	/*
		Vector of strings that can be matched.

		NOTE: we expect the internal strings to be postfix
		e.g. x + z * y -> +x*zy. This allows us to model a tree with strings.
	*/
	vector<string> strings; 

	PkbStatementPattern(shared_ptr<PkbStatementEntity> statement, vector<string> strings) {
		this->strings = strings;
		this->statement = statement;
	}

public :
	bool isAssignPattern() {
		return this->statement->isAssignStatement();
	}

	/*
		Takes in a vector of regex strings and returns true if all strings 
		match on all strings within the pattern.

		NOTE: we expect the internal strings to be postfix 
		e.g. x + z * y -> +x*zy. This allows us to model a tree with strings.
	*/
	bool isRegexMatch(vector<string> regexStrings) {
		if (regexStrings.size() != this->strings.size()) {
			throw PkbException("Trying to regex match, expecting exactly 2 regex strings, one for lhs and rhs.");
		}
		// iterate through and match
		int n = regexStrings.size();
		for (int i = 0; i < n; i++) {
			string regexString = regexStrings[i];
			string s = this->strings[i];
			if (!regex_match(s, regex(regexString))) {
				return false;
			}
		}
		return true;
	}

	/*
		Factory method that creates an assign pattern.
	*/
	static shared_ptr<PkbStatementPattern> createAssignPattern(int lineNumber, vector<string> strings) {
		if (strings.size() != 2) {
			throw PkbException("Trying to create assign pattern, expecting only 2 strings, lhs, and rhs.");
		}
		shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createAssignStatementEntity(lineNumber);
		return shared_ptr<PkbStatementPattern>(new PkbStatementPattern(statement, strings));
	}

	vector<string> getStringsToMatch() {
		return this->strings;
	}
	/*
		Gets a unique key for each pattern.
	*/
	string getKey() {
		string key;
		for (string pattern : this->strings) {
			key += pattern;
		}
		return key + this->statement->getKey();
	}

	int getStatementLineNumber() {
		return this->statement->getLineNumber();
	}

	string getVariableIdentifier() {
		return this->strings[0]; // we can confirm is lhs, since this always true for all patterns
	}

	bool equals(shared_ptr<PkbStatementPattern> other) {
		return this->statement->equals(other->statement) && this->strings == other->strings;
	}

};