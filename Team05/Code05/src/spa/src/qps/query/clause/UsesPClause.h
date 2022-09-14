#pragma once

#include "SuchThatClause.h"

class UsesPClause : public SuchThatClause {
public:
	shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
