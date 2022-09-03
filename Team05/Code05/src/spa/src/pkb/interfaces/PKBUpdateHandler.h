#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

using namespace std;

class PKBUpdateHandler {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    PKBUpdateHandler(ProgramKnowledgeBase pkb);

    // TODO Change string to super class of relationships
    void addRelationship(vector<Relationship> relationships);
    
    void addPattern(vector<Pattern> patterns);
    
    void addEntity(vector<Entity> entities);
};