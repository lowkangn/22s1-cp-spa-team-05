#pragma once

#include <assert.h>

#include <sp/dataclasses/cfg/exceptions/CFGException.h>
#include <sp/dataclasses/design_objects/Entity.h>

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

class CFGNode {
private:
    Entity entity;

protected:
    CFGNode() : entity(entity) {
    }

    explicit CFGNode(Entity entity) : entity(entity) {
        this->entity = entity;
    }

    vector<shared_ptr<CFGNode>> nextNodes;

public:
    // Default value for statementNumber is -1 which is also invalid
    static shared_ptr<CFGNode> createCFGNode(Entity entity) {
        return shared_ptr<CFGNode>(new CFGNode(entity));
    }

    vector<shared_ptr<CFGNode>> getChildren() {
        return this->nextNodes;
    }

    virtual shared_ptr<CFGNode> getNext() {
        return this->nextNodes[0];
    }

    virtual void addChild(shared_ptr<CFGNode> node) {
        // Node is not allowed to add itself as a child
        if (node->getLineNumber() != this->getLineNumber()) {
            this->nextNodes.push_back(node);
        }
    }

    bool equals(shared_ptr<CFGNode> other) {
        if (this->nextNodes.size() != other->nextNodes.size()) {
            return false;
        } else {
            for (int i = 0; i < this->nextNodes.size(); i++) {
                // To prevent infinite recursion in the case of while
                if (this->entity.getLine() < this->nextNodes[i]->entity.getLine()) {
                    // Recursively check child node
                    if (!(this->nextNodes[i]->equals(other->nextNodes[i]))) {
                        return false;
                    }
                } else {
                    // Recursive case where the child node is pointing to a parent while
                    if (!(this->nextNodes[i]->getLineNumber() == other->nextNodes[i]->
                        getLineNumber())) {
                        return false;
                    }
                }
            }
        }
        return this->entity == other->entity;
    }

    int getLineNumber() {
        return this->entity.getLine();
    }

    virtual bool isIfNode() {
        return false;
    }

    virtual bool isWhileNode() {
        return false;
    }

    virtual bool hasNext() {
        return this->nextNodes.size() != 0;
    }

    Entity getEntity() {
        return this->entity;
    }

    bool hasChildren() {
        return this->nextNodes.size() > 0;
    }

    static shared_ptr<CFGNode> createCFGFromAdjacencyList(
        unordered_map<int, shared_ptr<CFGNode>>& cfgNodes, unordered_map<int, vector<int>> adjList,
        int rootIndex) {
        // Add all the nodes
        for (pair<int, vector<int>> i : adjList) {
            int nodeToAddTo = i.first;
            vector<int> childrenIndexes = i.second;
            for (int nodeToAdd : childrenIndexes) {
                cfgNodes[nodeToAddTo]->addChild(cfgNodes[nodeToAdd]);
            }
        }

        // Return root
        return cfgNodes[rootIndex];
    }
};
