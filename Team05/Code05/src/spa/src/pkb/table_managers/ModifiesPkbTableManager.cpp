#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <string>
#include <vector>

std::string ModifiesPkbTableManager::filter(PkbClause clause) {
	return std::string{ "0" };
}

void ModifiesPkbTableManager::add(Relationship relationship) {

}

void ModifiesPkbTableManager::clearDataBase() {
	//Clear data
}
