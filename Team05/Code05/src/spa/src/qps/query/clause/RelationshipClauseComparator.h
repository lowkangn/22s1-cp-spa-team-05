#pragma once

//#include <qps/query/clause/AffectsClause.h>
//#include <qps/query/clause/AffectsTClause.h>
#include <qps/query/clause/NextClause.h>
#include <qps/query/clause/NextTClause.h>
#include <qps/query/clause/RelationshipClause.h>

#include <memory>

/* TODO(William): This class is incomplete / should not be used.
 * RelationshipClauseComparator is a functor that compares
 * 2 RelationshipClauses. RelationshipClauses are ordered as follows:
 
 */
class RelationshipClauseComparator {
public:
    int operator()(shared_ptr<RelationshipClause> left, shared_ptr<RelationshipClause> right) {
        if (left->requiresCfg() && !right->requiresCfg()) {
            return 0;
        } else if (!left->requiresCfg() && right->requiresCfg()) {
            return 1;
        }

        if (left->requiresCfg() && right->requiresCfg()) {
            return 1;
        }
        return 0;
    }
};