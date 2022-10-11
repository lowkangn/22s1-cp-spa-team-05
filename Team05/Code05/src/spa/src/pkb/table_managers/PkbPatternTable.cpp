#include <pkb/table_managers/PkbPatternTable.h>
#include <pkb/PkbException.h>

#include <iostream>

using namespace std;


string PkbPatternTable::add(shared_ptr<PkbPattern> pattern) {
	// get key
	string key = pattern->getKey();

	// check if item already in table
	if (this->table.find(key) != this->table.end()) {
		//cout << "Item already in table!";
	}

	// store
	this->table.insert({ key, pattern });
	return key;
}


shared_ptr<PkbPattern> PkbPatternTable::get(string& key) {
	if (this->table.find(key) == this->table.end()) {
		return NULL;
	}
	return this->table.at(key);
}

vector<shared_ptr<PkbPattern>> PkbPatternTable::getAll() {
	vector<shared_ptr<PkbPattern>> output;
	for (pair<string, shared_ptr<PkbPattern>> const& keyValuePair : this->table) {
		output.push_back(keyValuePair.second);
	}
	return output;
}

vector<shared_ptr<PkbPattern>> PkbPatternTable::getAllThatMatchPostFixStrings(vector<string> postFixStrings) {
		
	// get all 
	vector<shared_ptr<PkbPattern>> patterns = this->getAll();

	// convert string to regex strings
	vector<string> regexStrings;
	for (string s : postFixStrings) {
		regexStrings.push_back(this->createRegexStringFromString(s));
	}

	// do matching row by row
	vector<shared_ptr<PkbPattern>> output;
	for (shared_ptr<PkbPattern> p : patterns) {
		if (p->isRegexMatch(regexStrings)) {
			output.push_back(p);
		}
	}

	// return all matches
	return output;
}

vector<shared_ptr<PkbPattern>> PkbPatternTable::getVariableMatch(string conditions) {
	// get all 
	vector<shared_ptr<PkbPattern>> patterns = this->getAll();
	
	// If pattern to match is wildcard, set regex to to match all
	if (conditions == WILDCARD_CHAR) {
		return patterns;
	}
	else {
		vector<shared_ptr<PkbPattern>> output;
		// do matching row by row on the given condition
		for (shared_ptr<PkbPattern> p : patterns) {
			string varIdentifier = p->getVariableIdentifier();

			// if variable matches given condition, append to resulting vector
			if (varIdentifier == conditions) {
				output.push_back(p);
			}
		}
		return output;
	}
}
