#pragma once

#include <memory>
#include <assert.h>

#include <qps/query/clause/ClauseResult.h>
#include <pkb/interfaces/PKBQueryHandler.h>

class WithClause {
private:
public:

	WithClause() {}

	virtual shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) = 0;

	virtual bool equals(shared_ptr<WithClause> other) = 0;
};