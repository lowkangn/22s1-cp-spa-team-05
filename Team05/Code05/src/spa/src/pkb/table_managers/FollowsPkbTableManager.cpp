#include <pkb/table_managers/FollowsPkbTableManager.h>
#include <string>
#include <vector>

string FollowsPkbTableManager::filter(PkbClause clause) {
	return string{ "0" };
}

void FollowsPkbTableManager::add(Relationship relationship) {

}

void FollowsPkbTableManager::clearDataBase() {
	//Clear data
}
