#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <pkb/design_objects/entities/PkbEntity.h>
using namespace std;

class PkbGraphNode {

protected:
	shared_ptr<PkbEntity> entity; // encapsulated entity
	vector<shared_ptr<PkbGraphNode>> children; // children
	PkbGraphNode(shared_ptr<PkbEntity> entity) {
		this->entity = entity;
	}	

public:


	/*
		Adds a child to the node. Note that there is no notion of order in 
		the children.
	*/
	void addNeighbour(shared_ptr<PkbGraphNode> node) {
		this->children.push_back(node);
	}

	vector<shared_ptr<PkbGraphNode>> getNeighbours() {
		return this->children;
	}

	/*
		Returns a unique key identifying the node.
	*/
	virtual string getKey() = 0;

	/*
		Equality via concurrent bfs or an option of shallow comparison. Note that the deep comparison
		assumes that the keys of each node are unique.
	*/
	bool equals(shared_ptr<PkbGraphNode> other, bool shallow = false) {
		bool sameEntity = this->entity->equals(other->entity);
		bool shallowSameNeighbours = (this->children.size() == other->children.size());
		if (shallow) {
			return sameEntity && shallowSameNeighbours; // stop here
		}
		
		// bfs to get a set of all reachable components
		unordered_set<string> keys = this->getKeysOfAllConnectedNodes();
		unordered_set<string> otherKeys = this->getKeysOfAllConnectedNodes();

		// compare visited sets
		// NOTE: std== for unordered_set is not correct. see https://stackoverflow.com/questions/13158682/c-stdset-equality
		// for that reason, we need to manually iterate and check
		bool keysAreEqual = true;
		for (unordered_set<string>::iterator i = keys.begin(); i != keys.end(); ++i) {
			string key = *i;
			// check if inside
			if (otherKeys.count(key) == 0) {
				keysAreEqual = false;
				break;
			}
		}

		// in the other direction
		for (unordered_set<string>::iterator i = otherKeys.begin(); i != otherKeys.end(); ++i) {
			string key = *i;
			// check if inside
			if (keys.count(key) == 0) {
				keysAreEqual = false;
				break;
			}
		}

		return keysAreEqual && sameEntity && shallowSameNeighbours;

		
		
	}

	/*
		Gets the keys of all connected nodes (by transitivity). i.e. the keys of all nodes traversable 
		from this node will be included in this set.
	*/
	unordered_set<string> getKeysOfAllConnectedNodes() {

		// initialize for bfs
		unordered_set<string> visited;
		queue<shared_ptr<PkbGraphNode>> q;
		for (shared_ptr<PkbGraphNode> neighbour : this->getNeighbours()) {
			q.push(neighbour);
		}
		visited.insert(this->getKey());

		// do bfs
		while (!q.empty()) {
			// 1. pop 
			shared_ptr<PkbGraphNode> node = q.front();
			q.pop();

			// 2. mark as visited and check if target
			string key = node->getKey();
			visited.insert(key);

			// 3. for all neighbours, if not visited, add to queue
			for (shared_ptr<PkbGraphNode> neighbour : node->getNeighbours()) {
				string neighbourKey = neighbour->getKey();
				if (visited.find(neighbourKey) == visited.end()) { // if not inside
					q.push(neighbour);
				}
			}
		}
		return visited; 
	}

	// =========== factories =============
	/*
		Constructs a graph from the adjlist.
	*/
	static shared_ptr<PkbGraphNode> createGraphFromAdjList(int rootNodeId, vector<vector<int>> adjList, unordered_map<int,shared_ptr<PkbGraphNode>> nodeIdToNode) {
		// 1. for each node in adjlist, add neighbours
		int n = adjList.size();
		for (int i = 0; i < n; i++) {
			vector<int> neighbours = adjList[i];
			shared_ptr<PkbGraphNode> node = nodeIdToNode.at(i);

			for (int j : neighbours) {
				node->addNeighbour(nodeIdToNode.at(j));
			}
		}

		// 2. return specified root node
		return nodeIdToNode.at(rootNodeId);
	}
};