#pragma once

#include <pkb/design_objects/entities/PkbEntity.h>
#include <pkb/PkbException.h>

#include <unordered_map>
#include <string>
#include <memory>
using namespace std;

class PkbEntityTable {
private: 
	unordered_map<string, shared_ptr<PkbEntity>> table;
public:

	PkbEntityTable() {};

	/*
		Adds the entity and returns the key used to store it.
		Throws an error if a duplicate object is attempted to be stored.
	*/
	string add(shared_ptr<PkbEntity> entity) {
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

	/*
		Gets an item by a specific key. If the item is not present, 
		we return NULL.
	*/
	shared_ptr<PkbEntity> get(string &key) {
		if (this->table.find(key) == this->table.end()) {
			return NULL;
		}
		return this->table.at(key);
	}

};