#include <list>
#include <map>
#include "../dataclasses/Entity.h"
#include "../dataclasses/Relationship.h"
#include "Manager.h"

using namespace std;

class FollowsTManager : public Manager
{
private:
	map<Entity, list<Relationship>> mappings;
};
