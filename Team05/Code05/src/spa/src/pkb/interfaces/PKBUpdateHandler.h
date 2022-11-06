#pragma once

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>

#include <memory>
#include <string>
#include <vector>

using namespace std;

class PKBUpdateHandler {
public:
    virtual void addRelationships(vector<Relationship> relationships) = 0;

    virtual void addPatterns(vector<Pattern> patterns) = 0;

    virtual void addEntities(vector<Entity> entities) = 0;

    virtual void addCfgs(vector<shared_ptr<CFGNode>> rootNodes) = 0;
};
