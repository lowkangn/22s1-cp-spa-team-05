#include "Pattern.h"
#include "Entity.h"

bool Pattern::matchesPattern(Entity lhs, Entity rhs) {
	return this->lhs.equals(lhs) && this->rhs.equals(rhs);
}