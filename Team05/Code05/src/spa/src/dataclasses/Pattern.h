#include "Entity.h"

class Pattern
{
	public:
		bool matchesPattern(Entity statement);

	protected:
		Entity lhs;
		Entity rhs;
};
