#pragma once

#include <assert.h>

#include <pkb/interfaces/PKBQueryHandler.h>

#include <qps/query/clause/AttributeClause.h>
#include <qps/query/clause/ClauseResult.h>

#include <memory>

class WithClause : public AttributeClause {
public:
    WithClause() {
    }

    virtual shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) = 0;

    virtual bool equals(shared_ptr<WithClause> other) = 0;
};
