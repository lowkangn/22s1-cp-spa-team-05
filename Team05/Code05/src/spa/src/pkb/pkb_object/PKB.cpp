#pragma once

#include <vector>
#include <map>
#include <pkb/pkb_object/PKB.h>
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

#include <pkb/interfaces/PKBUpdateHandler.h>


using namespace std;

ProgramKnowledgeBase::ProgramKnowledgeBase() {
    entityManager = EntityPkbTableManager();
    // followsTable = new FollowsPkbTableManager(entityManager);
    // followsTTable = new FollowsTPkbTableManager(entityManager);
    modifiesTable = new ModifiesPkbTableManager(entityManager);
    // parentTable = new ParentPkbTableManager(entityManager);
    // parentTTable = new ParentTPkbTableManager(entityManager);
    // patternTable = new PatternPkbTableManager(entityManager);
    // usesTable = new UsesPkbTableManager(entityManager);
}

ProgramKnowledgeBase *ProgramKnowledgeBase::getInstance() {
    if (!PKBInstance) {
        PKBInstance = new ProgramKnowledgeBase;
    }
    return PKBInstance;
}

ProgramKnowledgeBase* ProgramKnowledgeBase::PKBInstance = NULL;

std::vector<Entity> ProgramKnowledgeBase::retrieveAllKnowledge() {
    return std::vector<Entity>();
}

Entity ProgramKnowledgeBase::retrieveKnowledge(PkbClause queryClause) {
    enum class clauseType {
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
    EntityPkbTableManager entityManager = EntityPkbTableManager();
    Token tokenX = Token("x", TokenType::NAME_OR_KEYWORD);
    EntityIdentifier identifierX = EntityIdentifier(tokenX, "x");
    Entity entityX = Entity(EntityType::VARIABLE, 4, tokenX, "x");
    return entityX;
}

// TODO Refactor to dataclass coming from QPS
void ProgramKnowledgeBase::deleteKnowledge(std::string knowledge) {

}

vector<int> ProgramKnowledgeBase::addRelationship(vector<Relationship> relationships) {
    vector<int> relationshipIds;
    for (auto &rs: relationships) {
        vector<Entity> entities = rs.getEntities();
        vector<int> entityIds = addEntity(entities);
        PkbRelationship *pkbRelationship = new PkbRelationship(entityIds[0], entityIds[1]);
        RelationshipType relationshipType = rs.getType();
        int rsId;
        switch (relationshipType) {
            case RelationshipType::MODIFIES:
                rsId = modifiesTable->add(*pkbRelationship);
                relationshipIds.push_back(rsId);
                break;
            default:
                break;
        }
    }
    return relationshipIds;
}

void ProgramKnowledgeBase::addPattern(vector<Pattern> patterns) {
    int x;
}

vector<int> ProgramKnowledgeBase::addEntity(vector<Entity> entities) {
    vector<int> entityIdVector;
    for (auto &entity : entities) {
        string entityStr = entity.toString();
        // TODO Cover other entity types
        if (entity.getType() == EntityType::STMT) {
            int lineNum = entity.getLine();
            PkbEntity pkbEntity = PkbEntity::generateStatement(entityStr, lineNum);
            int entityId = this->entityManager.add(pkbEntity);
            entityIdVector.push_back(entityId);
        } else {
            PkbEntity pkbEntity = PkbEntity::generateVariable(entityStr);
            int entityId = this->entityManager.add(pkbEntity);
            entityIdVector.push_back(entityId);
        }
    }
    return entityIdVector;
}

ModifiesPkbTableManager ProgramKnowledgeBase::getModifiesTable() {
    return *modifiesTable;
}

