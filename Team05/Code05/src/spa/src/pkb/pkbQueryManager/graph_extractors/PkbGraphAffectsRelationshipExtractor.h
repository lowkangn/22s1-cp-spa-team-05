#pragma once

#include <pkb/pkbQueryManager/PkbRelationshipQueryHelper.h>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class PkbGraphAffectsRelationshipExtractor {
private:
    unordered_set<string> cycleVisited;
    // set to track which nodes have been hit as a cycle before to avoid double cycles

public:
    /*
        Traverses the graph from a start node to an end node and checks if an affects relationship
        holds.
    */
    bool hasAffectsRelationship(
        shared_ptr<PkbControlFlowGraphNode> startNode, shared_ptr<PkbControlFlowGraphNode> endNode,
        shared_ptr<PkbRepository> repository,
        unordered_map<string, shared_ptr<PkbEntity>> candidateVariableKeyValueMap);
};
