#include <pkb/table_managers/ParentTPkbTableManager.h>
#include <string>
#include <vector>

string ParentTPkbTableManager::filter(PkbClause clause) {
	return string{ "0" };
}

void ParentTPkbTableManager::add(Relationship relationship) {

}

void ParentTPkbTableManager::clearDataBase() {
	//Clear data
}
