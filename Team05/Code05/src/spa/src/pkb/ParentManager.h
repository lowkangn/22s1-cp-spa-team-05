#include <map>
#include "../dataclasses/Entity.h"
#include "../dataclasses/Relationship.h"
#include "Manager.h"

using namespace std;

class ParentManager : public Manager
{
private:
	map<Entity, Relationship> mappings;
};
