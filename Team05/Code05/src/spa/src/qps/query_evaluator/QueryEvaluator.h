#pragma once

#include <pkb/PKB.h>

#include <qps/query/Query.h>
#include <qps/query/clause/EntityClauseResult.h>
#include <qps/query/clause/RelationshipClause.h>
#include <qps/query/clause/RelationshipClauseResult.h>
#include <qps/query/clause/WithClause.h>
#include <qps/query_evaluator/CfgClauseOptimiser.h>
#include <qps/query_evaluator/QueryResultsCombiner.h>
#include <qps/query_evaluator/QueryResultsOptimiser.h>

#include <algorithm>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class QueryEvaluator {
private:
    template <class T>
    list<ClauseResult> dereferenceResults(list<shared_ptr<T>> resultPointers) {
        static_assert(is_base_of<ClauseResult, T>::value, "T must be a subclass of ClauseResult");
        list<ClauseResult> results;
        for (const shared_ptr<ClauseResult>& resultPointer : resultPointers) {
            results.push_back(*resultPointer);
        }
        return results;
    }

public:
    QueryEvaluator() = default;

    ClauseResult evaluate(Query query, shared_ptr<PKBQueryHandler> pkb);
};
