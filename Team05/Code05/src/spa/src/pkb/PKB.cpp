#include <vector>
#include <map>
#include "PKB.h"
#include <sp/dataclasses/design_objects/Entity.h>
#include <qps/query/clause/Clause.h>
#include <pkb/table_managers/EntityPkbTableManager.h>
#include <pkb/table_managers/FollowsPkbTableManager.h>
#include <pkb/table_managers/FollowsTPkbTableManager.h>
#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <pkb/table_managers/ParentPkbTableManager.h>
#include <pkb/table_managers/ParentTPkbTableManager.h>
#include <pkb/table_managers/PatternPkbTableManager.h>
#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <pkb/table_managers/UsesPkbTableManager.h>

using namespace std;

ProgramKnowledgeBase::ProgramKnowledgeBase() {
    entityManager = EntityPkbTableManager();
    followsTable = new FollowsPkbTableManager(entityManager);
    followsTTable = new FollowsTPkbTableManager(entityManager);
    modifiesTable = new ModifiesPkbTableManager(entityManager);
    parentTable = new ParentPkbTableManager(entityManager);
    parentTTable = new ParentTPkbTableManager(entityManager);
    patternTable = new PatternPkbTableManager(entityManager);
    usesTable = new UsesPkbTableManager(entityManager);
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
//    switch(s_mapClauseTypes[queryClauseType]) {
//        case FollowsClause:
//            // stub argument into filter
//            tableManagers[1]->filter(PkbQuery());
//            break;
//        case FollowsTClause:
//            tableManagers[2]->filter(PkbQuery());
//            break;
//        case ModifiesPClause:
//        case ModifiesSClause:
//            tableManagers[3]->filter(PkbQuery());
//            break;
//        case ParentClause:
//            tableManagers[4]->filter(PkbQuery());
//            break;
//        case ParentTClause:
//            tableManagers[5]->filter(PkbQuery());
//            break;
//        case PatternClause:
//            tableManagers[6]->filter(PkbQuery());
//        case UsesPClause:
//        case UsesSClause:
//            tableManagers[7]->filter(PkbQuery());
//            break;
//    };
}

// TODO Refactor to dataclass coming from QPS
void ProgramKnowledgeBase::deleteKnowledge(std::string knowledge) {

}

void ProgramKnowledgeBase::addRelationship(vector<Relationship> relationships) {
    for (auto &rs: relationships) {
        vector<Entity> entities = rs.getEntities();
        vector<int> entityIds = addEntity(entities);
        PkbRelationship *pkbRelationship = new PkbRelationship(entityIds[0], entityIds[1]);
        RelationshipType relationshipType = rs.getType();
        switch (relationshipType) {
            case RelationshipType::MODIFIES:
                modifiesTable->add(*pkbRelationship);
                break;
            case RelationshipType::USES:
                usesTable->add(*pkbRelationship);
                break;
            case RelationshipType::FOLLOWS:
                followsTable->add(*pkbRelationship);
                break;
            case RelationshipType::FOLLOWST:
                followsTTable->add(*pkbRelationship);
                break;
            case RelationshipType::PARENT:
                parentTable->add(*pkbRelationship);
                break;
            case RelationshipType::PARENTT:
                parentTTable->add(*pkbRelationship);
                break;
            default:
                break;
        }
    }
}

vector<int> ProgramKnowledgeBase::addEntity(vector<Entity> entities) {
    vector<int> entityIdVector;
    for (auto &entity : entities) {
        string entityStr = entity.toString();
        PkbEntity *pkbEntity;
        // TODO Cover other entity types
        if (entity.getType() == EntityType::STMT) {
            int lineNum = entity.getLine();
            *pkbEntity = PkbEntity::generateStatement(entityStr, lineNum);
        } else {
            *pkbEntity = PkbEntity::generateVariable(entityStr);
        }
        int entityId = this->entityManager.add(*pkbEntity);
        entityIdVector.push_back(entityId);
    }
    return entityIdVector;
}


