#include <pkb/table_managers/FollowsTPkbTableManager.h>
#include <string>
#include <vector>

std::string FollowsTPkbTableManager::filter(PkbClause clause) {
	return std::string{ "0" };
}

void FollowsTPkbTableManager::add(Relationship relationship) {

}

void FollowsTPkbTableManager::clearDataBase() {
	//Clear data
}
