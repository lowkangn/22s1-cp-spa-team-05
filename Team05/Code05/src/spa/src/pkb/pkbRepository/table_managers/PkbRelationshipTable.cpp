#include <pkb/pkbRepository/table_managers/PkbRelationshipTable.h>
#include <pkb/PkbException.h>

#include <iostream>

using namespace std;


string PkbRelationshipTable::add(shared_ptr<PkbRelationship> relationship) {
	// get key
	string key = relationship->getKey();

	// check if item already in table
	if (this->table.find(key) != this->table.end()) {
		//cout << "Item already in table!";
	}

	// store
	this->table.insert({ key, relationship });
	return key;
}


shared_ptr<PkbRelationship> PkbRelationshipTable::get(string& key) {
	if (this->table.find(key) == this->table.end()) {
		return NULL;
	}
	return this->table.at(key);
}

vector<shared_ptr<PkbRelationship>> PkbRelationshipTable::getAll() {
	vector<shared_ptr<PkbRelationship>> output;
	for (pair<string, shared_ptr<PkbRelationship>> const& keyValuePair : this->table) {
		output.push_back(keyValuePair.second);
	}
	return output;
}