#pragma once

#include <pkb/design_objects/PkbClause.h>
#include <pkb/design_objects/PkbRelationship.h>
#include <pkb/table_managers/PkbTableManager.h>
#include <map>
#include <string>
#include <vector>

class RelationshipPkbTableManager: public PkbTableManager<PkbRelationship> {
public:
	virtual vector<int> filter(PkbClause clause) = 0;
};
