#include <pkb/table_managers/FollowsTPkbTableManager.h>
#include <string>
#include <vector>

string FollowsTPkbTableManager::filter(PkbClause clause) {
	return string{ "0" };
}

void FollowsTPkbTableManager::add(Relationship relationship) {

}

void FollowsTPkbTableManager::clearDataBase() {
	//Clear data
}
