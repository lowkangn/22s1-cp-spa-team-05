#include <pkb/table_managers/PkbRelationshipTable.h>
#include <pkb/PkbException.h>

using namespace std;


string PkbRelationshipTable::add(shared_ptr<PkbRelationship> entity) {
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


shared_ptr<PkbRelationship> PkbRelationshipTable::get(string& key) {
	if (this->table.find(key) == this->table.end()) {
		return NULL;
	}
	return this->table.at(key);
}
