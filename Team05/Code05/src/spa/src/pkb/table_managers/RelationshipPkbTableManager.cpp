#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <string>
#include <vector>

std::string RelationshipPkbTableManager::filter(PkbClause clause) {
	return std::string{ "0" };
}

void RelationshipPkbTableManager::add(Relationship relationship) {

}

void RelationshipPkbTableManager::clearDataBase() {
	//Clear data
}
