#include <list>
#include "Relationship.h"

using namespace std;

class AbstractQueryResult 
{
	protected:
		list<Relationship> foundRelationships;
};
