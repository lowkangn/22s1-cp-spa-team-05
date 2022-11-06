#pragma once

#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/entities/PkbEntity.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

class PkbEntityTable {
private:
    unordered_map<string, shared_ptr<PkbEntity>> table;

public:
    PkbEntityTable() {
    }

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

    /*
        Returns all items.
    */
    vector<shared_ptr<PkbEntity>> getAll();
};
