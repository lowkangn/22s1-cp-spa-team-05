#pragma once

#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/RelationshipClauseResult.h>

/* WithNoExactClause encapsulates a with clause
	that returns a relationship result.
*/
class WithNoExactClause : public WithClause {
private:
	vector<ClauseArgument> lhsArgs;
	vector<ClauseArgument> rhsArgs;

public:
	WithNoExactClause(vector<ClauseArgument> lhsArgs, vector<ClauseArgument> rhsArgs) 
		: lhsArgs(lhsArgs), rhsArgs(rhsArgs) {};

	shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

	bool equals(shared_ptr<WithClause> other) override;
};