#pragma once

#include <qps/query/clause/RelationshipClause.h>

class UsesPClause : public RelationshipClause {
public:
	shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;
};
