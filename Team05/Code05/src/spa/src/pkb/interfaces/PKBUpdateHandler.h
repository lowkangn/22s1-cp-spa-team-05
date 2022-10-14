#pragma once
#include <string>
#include <vector>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <pkb/design_objects/graphs/PkbGraphNode.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <memory>

using namespace std;

class PKBUpdateHandler {
public:

    virtual void addRelationships(vector<Relationship> relationships) = 0;
    
    virtual void addPatterns(vector<Pattern> patterns) = 0;
    
    virtual void addEntities(vector<Entity> entities) = 0;

    virtual void addCfg(shared_ptr<CFGNode> rootNode) = 0;
};

