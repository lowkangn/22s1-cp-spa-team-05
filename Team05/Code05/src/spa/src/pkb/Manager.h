#include <list>
#include "../dataclasses/QueryClause.h"
#include "../dataclasses/Relationship.h"

using namespace std;

class Manager 
{
	public:
		virtual list<Relationship> filter(QueryClause queryClause);

		virtual void add(Relationship relationship);

		virtual void clear();
};
