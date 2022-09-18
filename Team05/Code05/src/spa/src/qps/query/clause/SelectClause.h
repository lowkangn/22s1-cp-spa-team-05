#pragma once

#include <memory>
#include <string>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/EntityClauseResult.h>
#include <qps/exceptions/PQLSyntaxError.h>
#include <pkb/interfaces/PKBQueryHandler.h>

using namespace std;

class SelectClause {
private:
	ClauseArgument toSelect;
public:
	SelectClause(ClauseArgument toSelect) : toSelect(toSelect) {};
	shared_ptr<EntityClauseResult> execute(shared_ptr<PKBQueryHandler> pkb);
	bool equals(const SelectClause* other);
};
