#pragma once

#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/RelationshipClauseResult.h>

/* WithRelationshipClause encapsulates a with clause
	that returns a relationship result.
*/
class WithRelationshipClause : public WithClause {
private:
	vector<ClauseArgument> lhsArgs;
	vector<ClauseArgument> rhsArgs;
	bool hasExactArg;

public:
	WithRelationshipClause(vector<ClauseArgument> lhsArgs, vector<ClauseArgument> rhsArgs) 
		: lhsArgs(lhsArgs), rhsArgs(rhsArgs), hasExactArg(false) {};

	WithRelationshipClause(ClauseArgument exactArg, vector<ClauseArgument> nonExactArgs)
		: lhsArgs({ exactArg }), rhsArgs(rhsArgs), hasExactArg(true) {};

	shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

	bool equals(shared_ptr<WithClause> other) override;
};