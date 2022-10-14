#pragma once

#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClauseResult.h>

/* WithNoExactClause encapsulates a with clause
	whose both sides are not exact references 
	(i.e. are attrRefs, e.g. v.varName = c.procName)
*/
class WithNoExactClause : public WithClause {
private:
	ClauseArgument lhsSynonym;
	ClauseArgument lhsAttribute;
	ClauseArgument rhsSynonym;
	ClauseArgument rhsAttribute;

	/* Handles execution when both sides are exactly the same (e.g. v.varName = v.varName)*/
	shared_ptr<EntityClauseResult> handleBothSidesEqual();

	/* Handles execution when both sides have a stmt# attribute (e.g. s.stmt# = r.stmt#) */
	shared_ptr<ClauseResult> handleBothSidesStmtNumAttribute(shared_ptr<PKBQueryHandler> pkb);

	/* Returns the result of retrieving from the pkb for one side of the with clause */
	shared_ptr<ClauseResult> retrieveForOneAttrRef(shared_ptr<PKBQueryHandler> pkb, 
		ClauseArgument& oneSideSynonym, ClauseArgument& oneSideAttribute);

	/* Returns the column which should be used to perform equality check on. */
	ClauseArgument getEqualityCheckColumn(ClauseArgument& oneSideSynonym, ClauseArgument& oneSideAttribute);

public:
	WithNoExactClause(vector<ClauseArgument> lhsArgs, vector<ClauseArgument> rhsArgs) 
		: lhsSynonym(lhsArgs.front()), lhsAttribute(lhsArgs.back()), 
		rhsSynonym(rhsArgs.front()), rhsAttribute(rhsArgs.back()) {
		assert(lhsArgs.size() == 2 && rhsArgs.size() == 2);
		assert(lhsArgs.front().isSynonym() && lhsArgs.back().isAttributeName());
		assert(rhsArgs.front().isSynonym() && rhsArgs.back().isAttributeName());
	};

	shared_ptr<ClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

	bool equals(shared_ptr<WithClause> other) override;
};