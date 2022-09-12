#pragma once

#include <pkb/design_objects/entities/PkbEntity.h>

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
	string add(shared_ptr<PkbEntity> entity);

	/*
		Gets an item by a specific key. If the item is not present, 
		we return NULL.
	*/
	shared_ptr<PkbEntity> get(string& key);

};