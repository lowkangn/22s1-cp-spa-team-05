#pragma once
#include <string>
#include <vector>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <pkb/PKB.h>

using namespace std;

class PKBUpdateHandler {
public:

    virtual void addRelationship(vector<Relationship> relationships);
    
    virtual void addPattern(vector<Pattern> patterns);
    
    virtual vector<int> addEntity(vector<Entity> entities);
};

