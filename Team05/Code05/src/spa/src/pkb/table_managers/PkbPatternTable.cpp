#include <pkb/table_managers/PkbPatternTable.h>
#include <pkb/PkbException.h>

#include <iostream>

using namespace std;


string PkbPatternTable::add(shared_ptr<PkbStatementPattern> pattern) {
	// get key
	string key = pattern->getKey();

	// check if item already in table
	if (this->table.find(key) != this->table.end()) {
		cout << "Item already in table!";
	}

	// store
	this->table.insert({ key, pattern });
	return key;
}


shared_ptr<PkbStatementPattern> PkbPatternTable::get(string& key) {
	if (this->table.find(key) == this->table.end()) {
		return NULL;
	}
	return this->table.at(key);
}

vector<shared_ptr<PkbStatementPattern>> PkbPatternTable::getAll() {
	vector<shared_ptr<PkbStatementPattern>> output;
	for (pair<string, shared_ptr<PkbStatementPattern>> const& keyValuePair : this->table) {
		output.push_back(keyValuePair.second);
	}
	return output;
}

vector<shared_ptr<PkbStatementPattern>> PkbPatternTable::getAllThatMatchPostFixStrings(vector<string> postFixStrings) {
		
	// get all 
	vector<shared_ptr<PkbStatementPattern>> patterns = this->getAll();

	// convert string to regex strings
	vector<string> regexStrings;
	for (string s : postFixStrings) {
		regexStrings.push_back(this->createRegexStringFromString(s));
	}

	// do matching row by row
	vector<shared_ptr<PkbStatementPattern>> output;
	for (shared_ptr<PkbStatementPattern> p : patterns) {
		if (p->isRegexMatch(regexStrings)) {
			output.push_back(p);
		}
	}

	// return all matches
	return output;
}