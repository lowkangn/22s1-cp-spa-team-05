#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <qps/query/clause/Clause.h>


using namespace std;

class ProgramKnowledgeBase {
private:
    // TODO Change to Entity-Relationship mapping once hashing functionality is fleshed out
    unordered_map<int, string> entityToRelationshipMap;
    unordered_map<int, string> relationshipToEntityMap;
    static ProgramKnowledgeBase* PKBInstance;

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

