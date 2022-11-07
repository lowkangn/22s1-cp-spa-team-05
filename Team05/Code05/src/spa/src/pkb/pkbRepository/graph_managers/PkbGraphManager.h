#pragma once

#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbGraphNode.h>

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class PkbGraphManager {
private:
    unordered_map<string, shared_ptr<PkbGraphNode>> keyToNodeMap; // maps key -> node
    shared_ptr<PkbGraphNode> rootNode; // rootNode

public:
    PkbGraphManager() {
    }

    /*
        Constructor initializing a manager with a root node.
        It will carry out graph traversal to map out the full graph internally.
    */
    explicit PkbGraphManager(shared_ptr<PkbGraphNode> rootNode) {
        // assign root node
        this->rootNode = rootNode;

        // initialize for bfs
        unordered_set<string> visited;
        queue<shared_ptr<PkbGraphNode>> q;
        q.push(rootNode);

        // do bfs
        while (!q.empty()) {
            // 1. pop
            shared_ptr<PkbGraphNode> node = q.front();
            q.pop();

            // 2. mark as visited and add key to hashmap
            string key = node->getKey();
            visited.insert(key);

            // 2b. if already inside, error!
            if (this->getNode(key) != NULL && (!this->getNode(key)->equals(node))) {
                throw PkbException(
                    "Key collision. Trying to add node, but different node with same key already inside."); //NOLINT(whitespace/line_length)
            }
            this->keyToNodeMap[key] = node;

            // 3. for all neighbours, if not visited, add to queue
            for (shared_ptr<PkbGraphNode> neighbour : node->getNeighbours()) {
                string neighbourKey = neighbour->getKey();
                if (visited.find(neighbourKey) == visited.end()) {
                    // if not inside
                    q.push(neighbour);
                }
            }
        }
    }

    /*
        Gets the root node of the graph. We allow this past the abstraction barrier to allow
        external components to directly interface with the graph for traversal.
    */
    shared_ptr<PkbGraphNode> getRootNode() {
        return this->rootNode;
    }


    /*
        Checks if a node is inside.
    */
    bool isInside(string nodeKey);


    /*
        Lets you get the immediate neighbours of a given node, provided the node key.
    */
    vector<shared_ptr<PkbGraphNode>> getImmediateNeighbours(string nodeKey);

    /*
        Lets you get the node by key.
    */
    shared_ptr<PkbGraphNode> getNode(string nodeKey);


    /*
        Checks if nodes 1 and 2 (specified by keys) are connected.
    */
    bool canReachNodeBFromNodeA(string nodeAKey, string nodeBKey);

    /*
        Checks if node 2 is a neighbour of node 1. Expects
    */
    bool nodeBisNeighbourOfA(string nodeAkey, string nodeBKey);
};
