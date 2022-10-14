#pragma once

#include <optional>

#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClauseResult.h>

/* WithOneExactClause encapsulates a with clause that
	has exactly 1 arg that is an exact reference.
*/
class WithOneExactClause : public WithClause {
private:
	ClauseArgument exactArg;
	vector<ClauseArgument> nonExactArgs;

	/* Executes the clause when nonExactArgs is a statement with a name attribute (e.g. print.varName)*/
	shared_ptr<ClauseResult> executeForStmtWithNameAttribute(shared_ptr<PKBQueryHandler> pkb);


public:
	WithOneExactClause(ClauseArgument exactArg, vector<ClauseArgument> nonExactArgs)
		: exactArg(exactArg), nonExactArgs(nonExactArgs) {}
	
	shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

	bool equals(shared_ptr<WithClause> other) override;
};