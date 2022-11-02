#pragma once

#include <qps/query/Query.h>
#include <qps/query/clause/AffectsClause.h>
#include <qps/query/clause/AffectsTClause.h>
#include <qps/query/clause/NextClause.h>
#include <qps/query/clause/NextTClause.h>

/* Settings to tweak */
constexpr int AFFECTS_PENALTY = -100;
constexpr int STAR_PENALTY = -50;
constexpr int SYNONYM_PENALTY = -10;
constexpr int RESTRICTION_BONUS = 5;

class CfgClauseOptimiser {
private:
    Query& query;
    vector<vector<ClauseResult>>& nonCfgCombinedResults;
    unordered_map<shared_ptr<CfgRelationshipClause>, int> clauseToWeightMap;
    unordered_map<ClauseArgument, pair<int, int>>& argToGroupIndexMap;
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap;

    int getArgsWeight(const ClauseArgument& lhs, const ClauseArgument& rhs);

    /* Returns true if firstClause is less than (i.e. ordered before) secondClause */
    bool compare(const shared_ptr<CfgRelationshipClause>& firstClause,
        const shared_ptr<CfgRelationshipClause>& secondClause);

    void doCommonVisitation(const ClauseArgument& lhs, const ClauseArgument& rhs,
            int clauseTypeWeight, shared_ptr<CfgRelationshipClause> clause);

    int populateRestrictionMap(const ClauseArgument& arg);

public:
    CfgClauseOptimiser(Query& query, vector<vector<ClauseResult>>& nonCfgCombinedResults,
        unordered_map<ClauseArgument, pair<int, int>>& argToGroupIndexMap)
            : query(query), nonCfgCombinedResults(nonCfgCombinedResults),
            argToGroupIndexMap(argToGroupIndexMap), restrictionMap({}) {}

    /* Optimises the execution of CfgClauses in query.
     * Returns the restrictions on each ClauseArgument present in CfgClauses.
     */
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& optimise();

    void visitNextClause(const ClauseArgument& lhs, const ClauseArgument& rhs, 
        shared_ptr<NextClause> clause);
    void visitNextTClause(const ClauseArgument& lhs, const ClauseArgument& rhs, 
        shared_ptr<NextTClause> clause);
    void visitAffectsClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        shared_ptr<AffectsClause> clause);
    void visitAffectsTClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        shared_ptr<AffectsTClause> clause);

};

struct ClauseWeightComparator {
    const unordered_map<shared_ptr<CfgRelationshipClause>, int>& clauseToWeightMap;

    ClauseWeightComparator(const unordered_map<shared_ptr<CfgRelationshipClause>, int>& clauseToWeightMap)
        : clauseToWeightMap(clauseToWeightMap) {}

    bool operator()(const shared_ptr<CfgRelationshipClause>& lhs, const shared_ptr<CfgRelationshipClause>& rhs)const {
        return clauseToWeightMap.at(lhs) < clauseToWeightMap.at(rhs);
    }
};