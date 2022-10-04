#pragma once

#include <memory>
#include <string>
#include <list>
#include <set>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/EntityClauseResult.h>
#include <qps/exceptions/PQLSyntaxError.h>
#include <pkb/interfaces/PKBQueryHandler.h>
#include <pkb/PKB.h>

using namespace std;

class SelectClause {
private:
	list<ClauseArgument> selectArgs;
	bool isBooleanReturnType;

	SelectClause(list<ClauseArgument> selectArgs, bool isBooleanReturnType) : selectArgs(selectArgs), isBooleanReturnType(isBooleanReturnType) {};

public:

	static SelectClause createSynonymSelectClause(list<ClauseArgument> selectArgs) {
		return SelectClause(selectArgs, false);
	}

	static SelectClause createBooleanSelectClause() {
		return SelectClause(list<ClauseArgument>(), true);
	}

	bool checkIfBooleanReturnType() {
		return this->isBooleanReturnType;
	}

	list<shared_ptr<EntityClauseResult>> execute(shared_ptr<PKBQueryHandler> pkb);
	shared_ptr<EntityClauseResult> getSingleEntityResult(shared_ptr<PKBQueryHandler> pkb, ClauseArgument toSelect);
	bool equals(const SelectClause* other);
};
