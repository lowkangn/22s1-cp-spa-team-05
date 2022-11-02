#pragma once

#include <qps/query/clause/RelationshipClause.h>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class CfgRelationshipClause : public RelationshipClause {
protected:
    unordered_set<PQLEntity> lhsEntities;
    unordered_set<PQLEntity> rhsEntities;

    bool hasExecuted;
    bool updateRestriction;

    bool hasRestriction(unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap, ClauseArgument& arg);

    void updateEntities(vector<PQLRelationship>& retrievedRelationships);

    void updateRestrictionMap(unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap);

    void replaceSynonymByExactArg(vector<ClauseArgument>& oneSideArgs, ClauseArgument& originalSynonym,
        unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap);

    /* Decides if it is worth updating the restrictionMap using the retrieved relationships */
    bool isWorthUpdating(unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap,
        vector<PQLRelationship> retrievedRelationships);

    shared_ptr<RelationshipClauseResult> executeWithRestrictionHelper(shared_ptr<PKBQueryHandler>& pkb,
        unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap);

    virtual PKBTrackedRelationshipType getPkbTrackedRelationshipType() = 0;

public:
    CfgRelationshipClause(ClauseArgument lhs, ClauseArgument rhs)
        : RelationshipClause(lhs, rhs), hasExecuted(false), updateRestriction(false) {}

    shared_ptr<RelationshipClauseResult> executeWithRestriction(shared_ptr<PKBQueryHandler> pkb,
        unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap);

    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKBQueryHandler> pkb) override;

    /* Returns true iff execution requires information on the control flow */
    bool requiresCfg() override {
        return true;
    }
};