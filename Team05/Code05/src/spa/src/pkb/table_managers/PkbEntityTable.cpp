#include <pkb/table_managers/PkbEntityTable.h>


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
		// .find is simply a method applied to std::vector. If nothing is found, it returns the pointer to the
		// last element in the vector
		// see https://stackoverflow.com/questions/571394/how-to-find-out-if-an-item-is-present-in-a-stdvector
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