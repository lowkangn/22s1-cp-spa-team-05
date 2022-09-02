#include <pkb/table_managers/UsesPkbTableManager.h>
#include <string>
#include <vector>

std::string UsesPkbTableManager::filter(PkbClause clause) {
	return std::string{ "0" };
}

void UsesPkbTableManager::add(Relationship relationship) {

}

void UsesPkbTableManager::clearDataBase() {
	//Clear data
}
