#include <pkb/table_managers/PkbEntityTable.h>
#include <pkb/PkbException.h>

using namespace std;


string PkbEntityTable::add(shared_ptr<PkbEntity> entity) {
	// get key
	string key = entity->getKey();

	// check if item already in table
	if (this->table.find(key) != this->table.end()) {
		throw PkbException("Item already in table!");
	}

	// store
	this->table.insert({ key, entity });
	return key;
}

	
shared_ptr<PkbEntity> PkbEntityTable::get(string& key) {
	if (this->table.find(key) == this->table.end()) {
		return NULL;
	}
	return this->table.at(key);
}

vector<shared_ptr<PkbEntity>> PkbEntityTable::getAll() {
	vector<shared_ptr<PkbEntity>> output;
	for (pair<string, shared_ptr<PkbEntity>> const& keyValuePair : this->table) {
		output.push_back(keyValuePair.second);
	}
	return output;
}