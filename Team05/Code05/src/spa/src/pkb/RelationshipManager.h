#include <list>
#include "../dataclasses/Relationship.h"
#include "Manager.h"
#include "FollowsManager.h";

using namespace std;

class RelationshipManager: public Manager 
{
	public:
		void storeRelationship(Relationship relationship);

	private:
		FollowsManager followsManager;
};
