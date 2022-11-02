#include <qps/query_evaluator/CfgClauseOptimiser.h>

unordered_map<ClauseArgument, unordered_set<PQLEntity>>& CfgClauseOptimiser::optimise() {
    // Visit clauses to populate restriction map and calculate weight for sorting
    this->query.enableClauseOptimiserVisit(this);
    // Sort based on weights
    this->query.sortOptimisableClauses(ClauseWeightComparator(this->clauseToWeightMap));
    return this->restrictionMap;
}

void CfgClauseOptimiser::doCommonVisitation(const ClauseArgument& lhs, const ClauseArgument& rhs,
        int clauseTypeWeight, shared_ptr<CfgRelationshipClause> clause) {
    int weight = clauseTypeWeight + this->getArgsWeight(lhs, rhs);
    if (lhs == rhs) {
        weight += 2 * this->populateRestrictionMap(lhs);
    } else {
        weight += this->populateRestrictionMap(lhs);
        weight += this->populateRestrictionMap(rhs);
    }
    this->clauseToWeightMap.insert({ make_shared<CfgRelationshipClause>(clause), weight });
}

int CfgClauseOptimiser::populateRestrictionMap(const ClauseArgument& arg) {
    // if already restricted in previous clause, short circuit and return 1
    if (this->restrictionMap.find(arg) != this->restrictionMap.end()) {
        return 1;
    }
    // if not an existing synonym in nonCfgResults, cannot restrict so return 0
    if (this->argToGroupIndexMap.find(arg) == this->argToGroupIndexMap.end()) {
        return 0;
    }
    // populate the restriction map
    pair<int, int> groupIndices = this->argToGroupIndexMap.at(arg);
    ClauseResult groupResult = this->nonCfgCombinedResults[groupIndices.first][groupIndices.second];
    this->restrictionMap.insert({ arg, groupResult.getEntitySet(arg) });
    return 1;
}

void CfgClauseOptimiser::visitNextClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        shared_ptr<NextClause> clause) {
    this->doCommonVisitation(lhs, rhs, 0, clause);
}

void CfgClauseOptimiser::visitNextTClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        shared_ptr<NextTClause> clause) {
    this->doCommonVisitation(lhs, rhs, STAR_PENALTY, clause);
}

void CfgClauseOptimiser::visitAffectsClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        shared_ptr<AffectsClause> clause) {
    this->doCommonVisitation(lhs, rhs, AFFECTS_PENALTY, clause);
}

void CfgClauseOptimiser::visitAffectsTClause(const ClauseArgument& lhs, const ClauseArgument& rhs,
        shared_ptr<AffectsTClause> clause) {
    this->doCommonVisitation(lhs, rhs, STAR_PENALTY + AFFECTS_PENALTY, clause);
}

int CfgClauseOptimiser::getArgsWeight(const ClauseArgument& lhs, const ClauseArgument& rhs) {
    return (lhs.isSynonym() ? SYNONYM_PENALTY : 0) + (rhs.isSynonym() ? SYNONYM_PENALTY : 0);
}
