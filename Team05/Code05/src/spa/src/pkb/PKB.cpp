#include <vector>
#include <map>
#include "PKB.h"
#include <sp/dataclasses/design_objects/Entity.h>
#include <qps/query/clause/Clause.h>

using namespace std;

ProgramKnowledgeBase::ProgramKnowledgeBase() {

}

ProgramKnowledgeBase *ProgramKnowledgeBase::getInstance() {
    if (!PKBInstance) {
        PKBInstance = new ProgramKnowledgeBase;
    }
    return PKBInstance;
}

std::vector<Entity> ProgramKnowledgeBase::retrieveAllKnowledge() {
    return std::vector<Entity>();
}

Entity ProgramKnowledgeBase::retrieveKnowledge(Clause queryClause) {
    enum clauseType {
        FollowsClause,
        FollowsTClause,
        ModifiesPClause,
        ModifiesSClause,
        ParentClause,
        ParentTClause,
        PatternClause,
        UsesPClause,
        UsesSClause
    };

    map<string, clauseType> s_mapClauseTypes;

    string queryClauseType = typeid(queryClause).name();

    // TODO: Optimise this switch case
    switch(s_mapClauseTypes[queryClauseType]) {
        case FollowsClause:
            // stub argument into filter
            tableManagers[1]->filter(PkbQuery());
            break;
        case FollowsTClause:
            tableManagers[2]->filter(PkbQuery());
            break;
        case ModifiesPClause:
        case ModifiesSClause:
            tableManagers[3]->filter(PkbQuery());
            break;
        case ParentClause:
            tableManagers[4]->filter(PkbQuery());
            break;
        case ParentTClause:
            tableManagers[5]->filter(PkbQuery());
            break;
        case PatternClause:
            tableManagers[6]->filter(PkbQuery());
        case UsesPClause:
        case UsesSClause:
            tableManagers[7]->filter(PkbQuery());
            break;
    };
}

// TODO Refactor to dataclass coming from QPS
void ProgramKnowledgeBase::addKnowledge(std::string knowledge) {

}

// TODO Refactor to dataclass coming from QPS
void ProgramKnowledgeBase::deleteKnowledge(std::string knowledge) {

}


