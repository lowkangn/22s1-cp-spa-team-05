#include "../dataclasses/Entity.h"
#include "../dataclasses/Relationship.h"
#include "Manager.h"
#include <map>

class FollowsManager: public Manager {
public:
	std::string filter(QueryClause queryClause) override;

	void add(Relationship relationship) override;

	void clearDataBase() override;

private:
	std::map<Entity, Relationship> mappings;
};
