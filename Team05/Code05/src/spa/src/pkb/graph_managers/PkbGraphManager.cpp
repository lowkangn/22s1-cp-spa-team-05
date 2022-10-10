#include <pkb/graph_managers/PkbGraphManager.h>
#include <pkb/PkbException.h>

shared_ptr<PkbGraphNode> PkbGraphManager::getNode(string nodeKey) {

	// check that is inside
	if (this->keyToNodeMap.find(nodeKey) == this->keyToNodeMap.end()) {
		// not inside, return null
		return NULL;
	}
	
	// inside, just get
	return this->keyToNodeMap.at(nodeKey);
}

vector<shared_ptr<PkbGraphNode>> PkbGraphManager::getImmediateNeighbours(string nodeKey) {


	shared_ptr<PkbGraphNode> node =  this->getNode(nodeKey);

	// check if empty
	if (node == NULL) {
		throw PkbException("Trying to get immediate neighbours of node, but node does not exist.");
	}

	// not empty, get
	return node->getNeighbours();
	

}

bool PkbGraphManager::canReachNodeBFromNodeA(string nodeAKey, string nodeBKey) {

	// get nodes
	shared_ptr<PkbGraphNode> nodeA = this->getNode(nodeAKey);
	shared_ptr<PkbGraphNode> nodeB = this->getNode(nodeBKey);

	// if either do not exist, throw
	if (nodeA == NULL || nodeB == NULL) {
		throw PkbException("Trying to check if B can be reached from A, but either A or B do not exist.");
	}

	// is ok, do bfs to check
	// initialize for bfs
	unordered_set<string> visited;
	queue<shared_ptr<PkbGraphNode>> q = { nodeA };

	// do bfs
	while (!q.empty()) {
		// 1. pop 
		shared_ptr<PkbGraphNode> node = q.pop();

		// 2. mark as visited and check if target
		string key = node->getKey();
		visited.insert(key);
		if (key == nodeBKey) { // target found
			return true;
		}

		// 3. for all neighbours, if not visited, add to queue
		for (shared_ptr<PkbGraphNode> neighbour : node->getNeighbours()) {
			string neighbourKey = neighbour->getKey();
			if (visited.find(neighbourKey) == visited.end()) { // if not inside
				q.push(neighbour);
			}
		}
	}
	return false; // completed bfs but did not find
}

