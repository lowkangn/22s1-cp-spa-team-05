#pragma once

#include <pkb/design_objects/PkbClause.h>
#include <pkb/design_objects/PkbRelationship.h>
#include <pkb/design_objects/PkbResult.h>
#include <pkb/table_managers/PkbTableManager.h>
#include <map>
#include <string>
#include <vector>

class RelationshipPkbTableManager: public PkbTableManager {
public:
	virtual vector<int> filter(PkbClause clause) = 0;

	virtual void add(PkbRelationship relationship) = 0;
};
