#pragma once
#include <string>
#include <vector>
#include "../PKB.h"
#include "PKBQueryHandler.h"
#include "../../qps/query/clause/ClauseArgument.h"
#include "../../qps/query/clause/ClauseResult.h"
#include "../../qps/query/clause/PQLEntity.h"
#include "../../qps/query/clause/PQLRelationship.h"

enum class ClauseType {
    Follows,
    FollowsT,
    Parent,
    ParentT,
    UsesS,
    UsesP,
    ModifiesS,
    ModifiesP
};

/*
 * Interface used by QPS to retrieve data from PKB
 */
class PKBQueryHandler {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    PKBQueryHandler(ProgramKnowledgeBase pkb);

    // Accepts one ClauseArgument, only called by SelectClause
    vector<PQLEntity> retrieveKnowledge(ClauseArgument arg);

    // Accepts one string for type and two ClauseArgument for lhs and rhs, called by Modifies, Uses, Follows, Parent clauses
    vector<PQLRelationship> retrieveKnowledge(ClauseType clauseType, ClauseArgument lhs, ClauseArgument rhs);
};
