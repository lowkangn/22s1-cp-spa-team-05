#include "Entity.h"

class Pattern
{
	public:
		bool matchesPattern(Entity lhs, Entity rhs);

	protected:
		Entity lhs;
		Entity rhs;
};
