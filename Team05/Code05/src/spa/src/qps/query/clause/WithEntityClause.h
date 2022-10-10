#pragma once

#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/EntityClauseResult.h>

/* WithEntityClause encapsulates a with clause
	that returns at most 1 entity with exactly 1
	exact argument.
*/
class WithEntityClause : public WithClause {
private:
	ClauseArgument exactArg;
	vector<ClauseArgument> nonExactArgs;

	/* Gets the PKBTrackedStatementType corresponding to the synonym */
	PKBTrackedStatementType getPKBStmtType(ClauseArgument& stmtSynonymArg);

public:
	WithEntityClause(ClauseArgument exactArg, vector<ClauseArgument> nonExactArgs)
		: exactArg(exactArg), nonExactArgs(nonExactArgs) {}
	
	shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

	bool equals(shared_ptr<WithClause> other) override;
};