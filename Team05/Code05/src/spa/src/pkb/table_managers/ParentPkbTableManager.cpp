#include <pkb/table_managers/ParentPkbTableManager.h>
#include <string>
#include <vector>

std::string ParentPkbTableManager::filter(PkbClause clause) {
	return std::string{ "0" };
}

void ParentPkbTableManager::add(Relationship relationship) {

}

void ParentPkbTableManager::clearDataBase() {
	//Clear data
}
