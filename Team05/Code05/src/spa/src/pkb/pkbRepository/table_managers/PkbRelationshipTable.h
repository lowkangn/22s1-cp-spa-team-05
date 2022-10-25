#pragma once

#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
using namespace std;

class PkbRelationshipTable {
private:
	unordered_map<string, shared_ptr<PkbRelationship>> table;
public:

	PkbRelationshipTable() {};

	/*
		Adds the relationship and returns the key used to store it.
		Throws an error if a duplicate object is attempted to be stored.
	*/
	string add(shared_ptr<PkbRelationship> relationship);

	/*
		Gets an item by a specific key. If the item is not present,
		we return NULL.
	*/
	shared_ptr<PkbRelationship> get(string& key);

	/*
		Returns all items.
	*/
	vector<shared_ptr<PkbRelationship>> getAll();

};