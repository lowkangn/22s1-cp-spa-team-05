#pragma once

#include <pkb/design_objects/patterns/PkbStatementPattern.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
using namespace std;

class PkbPatternTable {
private:
	unordered_map<string, shared_ptr<PkbStatementPattern>> table;
public:

	PkbPatternTable() {};

	/*
		Adds the pattern and returns the key used to store it.
		Throws an error if a duplicate object is attempted to be stored.
	*/
	string add(shared_ptr<PkbStatementPattern> pattern);

	/*
		Gets an item by a specific key. If the item is not present,
		we return NULL.
	*/
	shared_ptr<PkbStatementPattern> get(string& key);

	/*
		Returns all items.
	*/
	vector<shared_ptr<PkbStatementPattern>> getAll();

};