#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <type_traits>

#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <qps/query/clause/RelationshipClause.h>
#include <qps/query/clause/CallsClause.h>
#include <qps/query/clause/CallsTClause.h>
#include <qps/query/clause/FollowsClause.h>
#include <qps/query/clause/FollowsTClause.h>
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>
#include <qps/query/clause/PatternClause.h>
#include <qps/query/clause/PatternAssignClause.h>
#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/WithNoExactClause.h>
#include <qps/query/clause/WithOneExactClause.h>
#include <qps/query/clause/WithBothExactClause.h>
#include <qps/query/Query.h>
#include <qps/query_evaluator/QueryResultsCombiner.h>
#include <qps/query_evaluator/QueryResultsOptimiser.h>
#include <pkb/PKB.h>

using namespace std;

class QueryEvaluator {
private:
	template<class T>
	list<ClauseResult> dereferenceResults(list<shared_ptr<T>> resultPointers) {
		static_assert(is_base_of<ClauseResult, T>::value, "T must be a subclass of ClauseResult");
		list<ClauseResult> results;
		for (const shared_ptr<ClauseResult>& resultPointer : resultPointers) {
			results.push_back(*resultPointer);
		}
		return results;
	}

public:
	QueryEvaluator() {};

    ClauseResult evaluate(Query query, shared_ptr<PKBQueryHandler> pkb);
};
