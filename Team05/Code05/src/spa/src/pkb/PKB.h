#pragma once
#include <string>
#include <vector>

#include <sp/dataclasses/design_objects/Entity.h>
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

class ProgramKnowledgeBase {
private:

    static ProgramKnowledgeBase* PKBInstance;
    vector<shared_ptr<PkbTableManager>> tableManagers {
        shared_ptr<PkbTableManager>(new EntityPkbTableManager()),
        shared_ptr<PkbTableManager>(new FollowsPkbTableManager()),
        shared_ptr<PkbTableManager>(new FollowsTPkbTableManager()),
        shared_ptr<PkbTableManager>(new ModifiesPkbTableManager()),
        shared_ptr<PkbTableManager>(new ParentPkbTableManager()),
        shared_ptr<PkbTableManager>(new ParentTPkbTableManager()),
        shared_ptr<PkbTableManager>(new PatternPkbTableManager()),
        shared_ptr<PkbTableManager>(new UsesPkbTableManager())
    };

protected:

    ProgramKnowledgeBase();

public:

    static ProgramKnowledgeBase *getInstance();

    // TODO Change from string to super class of Knowledge
    std::vector<Entity> retrieveAllKnowledge();

    // TODO Change return type from string to super class of Knowledge
    // TODO Change arg from string to QueryClause class
    Entity retrieveKnowledge(Clause queryClause);

    // TODO Change from string to super class of Knowledge
    void addKnowledge(std::string knowledge);

    // TODO Change from string to super class of Knowledge
    void deleteKnowledge(std::string knowledge);

};

