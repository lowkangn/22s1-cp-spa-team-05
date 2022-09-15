#include <pkb/table_managers/PkbPatternTable.h>
#include <pkb/PkbException.h>

using namespace std;


string PkbPatternTable::add(shared_ptr<PkbStatementPattern> pattern) {
	// get key
	string key = pattern->getKey();

	// check if item already in table
	if (this->table.find(key) != this->table.end()) {
		throw PkbException("Item already in table!");
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