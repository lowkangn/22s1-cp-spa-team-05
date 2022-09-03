#pragma once

#include <pkb/design_objects/PkbClause.h>
#include <unordered_map>
#include <string>

using namespace std;

template <typename T> class PkbTableManager {
public:
	// The return int represents a unique id assigned to the object to add.
	virtual int add(T toAdd);

	virtual void clearDataBase() = 0;

protected:
	int nextId = 0;
};
