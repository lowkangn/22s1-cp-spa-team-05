#include <pkb/table_managers/ParentPkbTableManager.h>
#include <string>
#include <vector>

string ParentPkbTableManager::filter(PkbClause clause) {
	return string{ "0" };
}

void ParentPkbTableManager::add(Relationship relationship) {

}

void ParentPkbTableManager::clearDataBase() {
	//Clear data
}
