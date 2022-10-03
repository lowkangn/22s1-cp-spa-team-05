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

vector<shared_ptr<PkbPattern>> PkbPatternTable::getVariableMatch(vector<string> conditions) {
	// get all 
	vector<shared_ptr<PkbPattern>> patterns = this->getAll();
	
	vector<string> regexControlVar{ };
	// If pattern to match is wildcard, set regex to .* to match all
	if (conditions[0] == WILDCARD_CHAR) {
		regexControlVar.push_back(".*");
	}
	else { // otherwise, just match the given variable
		regexControlVar.push_back("^" + conditions[0] + "$");
	}
	vector<shared_ptr<PkbPattern>> output;
	for (shared_ptr<PkbPattern> p : patterns) {
		string stringToMatch = p->getStringsToMatch()[0];
		if (p->isRegexMatch(regexControlVar)) {
			output.push_back(p);
		}
	}
	// return all matches
	return output;
}
