#pragma once

#include "RelationshipClause.h"

class UsesPClause : public RelationshipClause {
public:
	shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
