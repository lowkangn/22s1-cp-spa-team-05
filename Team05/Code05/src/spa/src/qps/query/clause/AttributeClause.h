#pragma once

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <pkb/interfaces/PKBQueryHandler.h>

class AttributeClause {
protected:
	/* Returns the result of retrieving from the pkb for one attrRef (synonym.AttrName) */
	shared_ptr<ClauseResult> getSingleAttrRefResult(shared_ptr<PKBQueryHandler> pkb,
		ClauseArgument& synonym, ClauseArgument& attribute);

	/* Gets the PKBStatementType corresponding to a statement synonym */
	PKBTrackedStatementType getPKBStmtType(ClauseArgument& stmtSynonymArg);

	AttributeClause() {};
};