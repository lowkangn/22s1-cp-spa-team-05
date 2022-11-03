#pragma once

#include <qps/query/Query.h>
#include <qps/query/clause/AffectsClause.h>
#include <qps/query/clause/AffectsTClause.h>
#include <qps/query/clause/NextClause.h>
#include <qps/query/clause/NextTClause.h>
#include <qps/query_evaluator/QueryResultsOptimiser.h>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/* Scores (tweak to experiment)
 * More negative score --> later execution
 * Currently, scores are set so that the order of execution is:
 *  Next --> Next* --> Affects --> Affects*
 * and within each clause type the order is:
 *  both exact/wildcard --> one exact/wildcard --> both synonyms
 */
constexpr int AFFECTS_PENALTY = -100;
constexpr int STAR_PENALTY = -50;
constexpr int SYNONYM_PENALTY = -20;
constexpr int RESTRICTION_BONUS = 5;

class CfgClauseOptimiser {
private:
    Query& query;
    vector<vector<ClauseResult>>& nonCfgCombinedResults;
    unordered_map<shared_ptr<CfgRelationshipClause>, int> clauseToScoreMap;
    QueryResultsOptimiser& resultsOptimiser;
    unordered_map<ClauseArgument, unordered_set<PQLEntity>> restrictionMap;

    /* Gets the contribution to a clause's weight from its arguments */
    int getArgsScore(const ClauseArgument& lhs, const ClauseArgument& rhs);

    /* Visits a CfgRelationshipClause to perform weight calculation and restriction */
    void doCommonVisitation(const ClauseArgument& lhs, const ClauseArgument& rhs,
            int clauseTypeScore, CfgRelationshipClause* clause);

    /* Populates the restriction map by checking whether the arg is present in
     * nonCfgCombinedResults.
     *
     * Returns 1 if arg has been restricted, 0 otherwise
     */
    int populateRestrictionMap(ClauseArgument arg);

public:
    CfgClauseOptimiser(Query& query, vector<vector<ClauseResult>>& nonCfgCombinedResults,
        QueryResultsOptimiser& resultsOptimiser)
            : query(query), nonCfgCombinedResults(nonCfgCombinedResults),
            resultsOptimiser(resultsOptimiser) {}

    /* Optimises the execution of CfgClauses in query.
     * Returns the restrictions on each ClauseArgument present in CfgClauses.
     */
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& optimise();

    void visitNextClause(const ClauseArgument& lhs, const ClauseArgument& rhs, 
        NextClause* clause);
    void visitNextTClause(const ClauseArgument& lhs, const ClauseArgument& rhs, 
        NextTClause* clause);
    void visitAffectsClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        AffectsClause* clause);
    void visitAffectsTClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        AffectsTClause* clause);

};