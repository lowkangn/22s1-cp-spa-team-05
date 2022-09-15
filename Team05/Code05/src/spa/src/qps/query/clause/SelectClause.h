#pragma once

#include <string>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/EntityClauseResult.h>
#include <pkb/PKB.h>

using namespace std;

class SelectClause {
private:
	ClauseArgument toSelect;
public:
	SelectClause(ClauseArgument toSelect) : toSelect(toSelect) {};
	shared_ptr<EntityClauseResult> execute(shared_ptr<PKB> pkb);
	bool equals(const SelectClause* other);
};
