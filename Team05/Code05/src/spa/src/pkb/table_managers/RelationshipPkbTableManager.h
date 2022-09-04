#pragma once

#include <pkb/design_objects/PkbClause.h>
#include <pkb/design_objects/PkbEntity.h>
#include <pkb/design_objects/PkbRelationship.h>
#include <pkb/table_managers/EntityPkbTableManager.h>
#include <pkb/table_managers/PkbTableManager.h>
#include <map>
#include <string>
#include <vector>

class RelationshipPkbTableManager: public PkbTableManager<PkbRelationship> {
public:
	// Returns a list of entity ids which satisfy the clause.
	virtual vector<int> filter(PkbClause clause) = 0;

protected:
	PkbEntityGenerator g;
	EntityPkbTableManager entityManager;
};
