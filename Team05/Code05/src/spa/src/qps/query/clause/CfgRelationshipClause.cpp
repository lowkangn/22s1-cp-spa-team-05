#include <qps/query/clause/CfgRelationshipClause.h>

shared_ptr<RelationshipClauseResult> CfgRelationshipClause::executeWithRestriction(
    shared_ptr<PKBQueryHandler> pkb,
        unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap) {
    shared_ptr<RelationshipClauseResult> out;
    if (this->hasRestriction(restrictionMap, this->lhs)
        || this->hasRestriction(restrictionMap, this->rhs)) {
        out = this->executeWithRestrictionHelper(pkb, restrictionMap);
    } else {
        out = this->execute(pkb);
    }
    this->hasExecuted = true;
    this->updateRestrictionMap(restrictionMap);
    return out;
}

shared_ptr<RelationshipClauseResult> CfgRelationshipClause::executeWithRestrictionHelper(
    shared_ptr<PKBQueryHandler>& pkb,
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap) {
    // Apply restrictions
    vector<ClauseArgument> asLhs = { this->lhs };
    this->replaceSynonymByExactArg(asLhs, this->lhs, restrictionMap);

    vector<PQLRelationship> relationships;
    PKBTrackedRelationshipType type = this->getPkbTrackedRelationshipType();
    if (this->lhs == this->rhs) {
        // no need to replace rhs explicitly, just use lhs as rhs
        for (ClauseArgument& lhsArg : asLhs) {
            vector<PQLRelationship> retrieved = pkb->retrieveRelationshipByTypeAndLhsRhs(
                type, lhsArg, lhsArg);
            relationships.insert(
                relationships.end(),
                std::make_move_iterator(retrieved.begin()),
                std::make_move_iterator(retrieved.end()));
        }
    } else {
        vector<ClauseArgument> asRhs = { this->rhs };
        this->replaceSynonymByExactArg(asRhs, this->rhs, restrictionMap);
        for (ClauseArgument& lhsArg : asLhs) {
            for (ClauseArgument& rhsArg : asRhs) {
                vector<PQLRelationship> retrieved = pkb->retrieveRelationshipByTypeAndLhsRhs(
                    type, lhsArg, rhsArg);
                relationships.insert(
                    relationships.end(),
                    std::make_move_iterator(retrieved.begin()),
                    std::make_move_iterator(retrieved.end()));
            }
        }
    }

    //Update entities for updating restriction map
    // updateEntities may get expensive, we only update if it is worth it
    if (this->isWorthUpdating(restrictionMap, relationships)) {
        this->updateEntities(relationships);
    }

    return make_shared<RelationshipClauseResult>(this->lhs, this->rhs, relationships);
}

shared_ptr<RelationshipClauseResult> CfgRelationshipClause::execute(
    shared_ptr<PKBQueryHandler> pkb) {
    //Query pkb
    vector<PQLRelationship> relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
        this->getPkbTrackedRelationshipType(), this->lhs, this->rhs);

    // Update entities for updating restriction map
    // (always worth updating since the synonyms were previously unrestricted)
    this->updateRestriction = true;
    this->updateEntities(relationships);

    return make_shared<RelationshipClauseResult>(this->lhs, this->rhs, relationships);
}

bool CfgRelationshipClause::hasRestriction(
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap,
    ClauseArgument& arg) {
    return (arg.isStmtSynonym() || arg.isAssignSynonym())
        && restrictionMap.find(arg) != restrictionMap.end();
}

void CfgRelationshipClause::replaceSynonymByExactArg(
    vector<ClauseArgument>& oneSideArgs,
    ClauseArgument& originalSynonym,
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap) {
    if (!originalSynonym.isSynonym()) {
        // cannot be replaced
        return;
    }
    assert(oneSideArgs.size() == 1 && oneSideArgs.front() == originalSynonym);

    unordered_map<ClauseArgument, unordered_set<PQLEntity>>::const_iterator mapIter =
        restrictionMap.find(originalSynonym);
    if (mapIter == restrictionMap.end()) {
        // nothing to replace
        return;
    }

    // Replace with exact arg
    oneSideArgs.pop_back();
    for (const PQLEntity& entity : mapIter->second) {
        assert(entity.isStatement());
        oneSideArgs.emplace_back(ClauseArgument::createLineNumberArg(entity.toString()));
    }
}

void CfgRelationshipClause::updateEntities(vector<PQLRelationship>& retrievedRelationships) {
    for (const PQLRelationship& relationship : retrievedRelationships) {
        if (this->lhs.isSynonym() && this->updateRestriction) {
            this->lhsEntities.insert(relationship.getFirstEntity());
        }
        if (this->lhs == this->rhs) {
            continue;
        }
        if (this->rhs.isSynonym() && this->updateRestriction) {
            this->rhsEntities.insert(relationship.getSecondEntity());
        }
    }
}

void CfgRelationshipClause::updateRestrictionMap(
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap) {
    assert(this->hasExecuted);
    if (this->lhs.isSynonym() && this->updateRestriction) {
        restrictionMap.insert_or_assign(this->lhs, this->lhsEntities);
    }
    if (this->lhs == this->rhs) {
        return;
    }
    if (this->rhs.isSynonym() && this->updateRestriction) {
        restrictionMap.insert_or_assign(this->rhs, this->rhsEntities);
    }
}

bool CfgRelationshipClause::isWorthUpdating(
    unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap,
    vector<PQLRelationship>& retrievedRelationships) {
    assert(this->lhs.isSynonym() || this->rhs.isSynonym());
    // In general, the restriction is worth updating when the new number of
    // possible values for the synonym is strictly smaller

    // If one side is a non-synonym, the number of relationships is equal to the number of
    // remaining values for the synonym (assuming wildcard is optimised)
    if (this->lhs.isSynonym() && (this->rhs.isLineNumber()|| this->rhs.isWildcard())) {
        assert(restrictionMap.find(this->lhs) != restrictionMap.end());
        this->updateRestriction =
            restrictionMap.at(this->lhs).size() > retrievedRelationships.size();
        return this->updateRestriction;
    } else if (this->rhs.isSynonym() && (this->lhs.isLineNumber() || this->lhs.isWildcard())) {
        assert(restrictionMap.find(this->rhs) != restrictionMap.end());
        this->updateRestriction =
            restrictionMap.at(this->rhs).size() > retrievedRelationships.size();
        return this->updateRestriction;
    }

    // both are synonyms
    // if one is not restricted already, it is worth restricting it
    if (!this->hasRestriction(restrictionMap, this->lhs)
        || !this->hasRestriction(restrictionMap, this->rhs)) {
        this->updateRestriction = true;
        return this->updateRestriction;
    }

    int maxNumRelationships =
        restrictionMap.at(this->lhs).size() * restrictionMap.at(this->rhs).size();
    this->updateRestriction = maxNumRelationships > retrievedRelationships.size();
    return this->updateRestriction;
}

bool operator<(shared_ptr<CfgRelationshipClause> first,
    shared_ptr<CfgRelationshipClause> second) {
    return second->clauseWeight < first->clauseWeight;
}
