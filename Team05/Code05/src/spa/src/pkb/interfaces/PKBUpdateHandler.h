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

    void addRelationship(vector<Relationship> relationships);
    
    void addPattern(vector<Pattern> patterns);
    
    void addEntity(vector<Entity> entities);
};