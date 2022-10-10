#pragma once

#include <pkb/design_objects/graphs/PkbGraphNode.h>
#include <pkb/PkbException.h>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

class PkbGraphManager {
private:
	unordered_map<string, shared_ptr<PkbGraphNode>> keyToNodeMap; // maps key -> node

public:
	/*
		Default constructor. Initializes an empty manager.
	*/
	PkbGraphManager() {};

	/*
		Initializes a manager with a hashmap of keys pointing to nodes.
	*/
	PkbGraphManager(unordered_map<string, shared_ptr<PkbGraphNode>> keyToNodeMap) {
		this->keyToNodeMap = keyToNodeMap;
	}

	/*
		Constructor initializing a manager with a root node. It will carry out graph traversal to map
		out the full graph internally.
	*/
	PkbGraphManager(shared_ptr<PkbGraphNode> rootNode) {

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
				throw PkbException("Key collision. Trying to add node, but different node with same key already inside.");
			}
			this->keyToNodeMap[key] = node;

			// 3. for all neighbours, if not visited, add to queue
			for (shared_ptr<PkbGraphNode> neighbour : node->getNeighbours()) {
				string neighbourKey = neighbour->getKey();
				if (visited.find(neighbourKey) == visited.end()) { // if not inside
					q.push(neighbour);
				}
			}
		}

	}




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

};