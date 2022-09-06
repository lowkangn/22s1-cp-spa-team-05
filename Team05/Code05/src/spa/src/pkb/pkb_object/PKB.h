#pragma once

#include <pkb/interfaces/PKBUpdateHandler.h>

#include <string>
#include <vector>
#include <unordered_map>

#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <pkb/table_managers/PkbTableManager.h>
#include <pkb/table_managers/EntityPkbTableManager.h>
#include <pkb/table_managers/FollowsPkbTableManager.h>
#include <pkb/table_managers/FollowsTPkbTableManager.h>
#include <pkb/table_managers/ModifiesPkbTableManager.h>
#include <pkb/table_managers/ParentPkbTableManager.h>
#include <pkb/table_managers/ParentTPkbTableManager.h>
#include <pkb/table_managers/PatternPkbTableManager.h>
#include <pkb/table_managers/RelationshipPkbTableManager.h>
#include <pkb/table_managers/UsesPkbTableManager.h>

#include <qps/query/clause/Clause.h>


using namespace std;

class ProgramKnowledgeBase: public PKBUpdateHandler {
private:

    static shared_ptr<ProgramKnowledgeBase> PKBInstance;
    shared_ptr <EntityPkbTableManager> entityManager;
    shared_ptr<RelationshipPkbTableManager> modifiesTable;


    ProgramKnowledgeBase();

public:

    static shared_ptr<ProgramKnowledgeBase> getInstance();

    // TODO Change from string to super class of Knowledge
    vector<Entity> retrieveAllKnowledge();

    // TODO Change return type from string to super class of Knowledge
    // TODO Change arg from string to QueryClause class
    Entity retrieveKnowledge(PkbClause queryClause);

    vector<int> addRelationship(vector<Relationship> relationships) override;

    void addPattern(vector<Pattern> patterns) override;

    vector<int> addEntity(vector<Entity> entities) override;

    // TODO Change from string to super class of Knowledge
    void deleteKnowledge(std::string knowledge);

    shared_ptr<ModifiesPkbTableManager> _getModifiesTable();
    shared_ptr<EntityPkbTableManager> _getEntitiesTable();


    /*
        Helper method for testing purposes.
    */
    //vector<shared_ptr<Relationship>> _getAllRelationships(); 

    /*
        Helper method for testing purposes.
    */
    //vector<shared_ptr<Entity>> _getAllEntities();


};

