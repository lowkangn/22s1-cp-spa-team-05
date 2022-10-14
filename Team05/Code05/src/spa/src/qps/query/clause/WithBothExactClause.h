#pragma once

#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/EntityClauseResult.h>

/* WithTrivialClause encapsulates a with clause
	whose lhs and rhs are either both exact references,
	or both the same syn.AttrName
	e.g. `with 1 = 1`, `with v.varName = v.varName`
*/
class WithBothExactClause : public WithClause {
private:
	ClauseArgument lhs;
	ClauseArgument rhs;
public:
	WithBothExactClause(ClauseArgument lhs, ClauseArgument rhs) : lhs(lhs), rhs(rhs) {}

	shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

	bool equals(shared_ptr<WithClause> other) override;
};