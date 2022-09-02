#include <pkb/table_managers/FollowsPkbTableManager.h>
#include <string>
#include <vector>

std::string FollowsPkbTableManager::filter(PkbClause clause) {
	return std::string{ "0" };
}

void FollowsPkbTableManager::add(Relationship relationship) {

}

void FollowsPkbTableManager::clearDataBase() {
	//Clear data
}
