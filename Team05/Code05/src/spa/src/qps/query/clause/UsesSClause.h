#pragma once

#include "SuchThatClause.h"

class UsesSClause : public SuchThatClause {
public:
	shared_ptr<RelationshipClauseResult> execute() override;
};
