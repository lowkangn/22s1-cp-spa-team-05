#pragma once

#include "SuchThatClause.h"

class ParentClause : public SuchThatClause {
public:
    shared_ptr<RelationshipClauseResult> execute() override;
};
