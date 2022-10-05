#pragma once

#include <memory>
#include <assert.h>

#include <qps/query/clause/RelationshipClauseResult.h>
#include <pkb/interfaces/PKBQueryHandler.h>

class WithClause {
private:
	vector<ClauseArgument> lhsArgs;
	vector<ClauseArgument> rhsArgs;

	RelationshipClauseResult executeBothExact(shared_ptr<PKBQueryHandler> pkb);

	RelationshipClauseResult executeOnlyOneExactTrivial(shared_ptr<PKBQueryHandler> pkb);

	RelationshipClauseResult executeOnlyOneExact(shared_ptr<PKBQueryHandler> pkb);

	RelationshipClauseResult executeNoExact(shared_ptr<PKBQueryHandler> pkb);

	PQLEntity retrieveEntityByExactArg(shared_ptr<PKBQueryHandler> pkb, ClauseArgument& synonymArg, ClauseArgument& exactArg);

	PKBTrackedStatementType getPKBStmtType(ClauseArgument& stmtSynonymArg);

public:

	WithClause(vector<ClauseArgument> lhsArgs, vector<ClauseArgument> rhsArgs)
		: lhsArgs(lhsArgs), rhsArgs(rhsArgs) {}

	RelationshipClauseResult execute(shared_ptr<PKBQueryHandler> pkb);

	bool equals(shared_ptr<WithClause> other);
};