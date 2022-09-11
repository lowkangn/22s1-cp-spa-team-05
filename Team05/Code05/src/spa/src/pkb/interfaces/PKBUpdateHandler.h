#pragma once
#include <string>
#include <vector>
#include <pkb/ProgramKnowledgeBase.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;

class PKBUpdateHandler {
public:

    virtual vector<int> addRelationships(vector<Relationship> relationships) = 0;
    
    virtual void addPatterns(vector<Pattern> patterns) = 0;
    
    virtual vector<int> addEntities(vector<Entity> entities) = 0;
};

