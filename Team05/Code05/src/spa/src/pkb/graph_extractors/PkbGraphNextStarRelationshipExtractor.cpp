#include <pkb/graph_extractors/PkbGraphNextStarRelationshipExtractor.h>
#include <pkb/design_objects/relationships/PkbNextStarRelationship.h>

vector<shared_ptr<PkbGraphNode>> PkbGraphNextStarRelationshipExtractor::extractAllFromStartDfs(shared_ptr<PkbGraphNode> startNode) {

	// check if there are any neighbours left 
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();
	vector<shared_ptr<PkbGraphNode>> unvisited;
	string key = startNode->getKey();
	for (shared_ptr<PkbGraphNode> n : neighbours) {
		if (!this->visitedEdges.count(pair<string, string>(key, n->getKey()))) {
			unvisited.push_back(n);
		}
	}

	// if no neighbours left, return itself in vector
	if (unvisited.size() == 0) {
		return vector<shared_ptr<PkbGraphNode>>{startNode};
	}
	// if have, for each neighbour, add to visited list, get all child from vector 
	vector<shared_ptr<PkbGraphNode>> allDownstream;
	for (shared_ptr<PkbGraphNode> n : unvisited) {
		// get all downstream children
		vector<shared_ptr<PkbGraphNode>> downstream = this->extractAllFromStartDfs(n);

		// for each downstream child, form relationship with self and it
		for (shared_ptr<PkbGraphNode> c : downstream) {
			// create relationship and add
			this->extractedRelationships.push_back(shared_ptr<PkbRelationship>(new PkbNextStarRelationship(startNode->getAsEntity(), c->getAsEntity())));
		}
	
		// extend
		allDownstream.insert(allDownstream.end(), downstream.begin(), downstream.end());
	}

	// set state
	this->extracted = true;

	return allDownstream;
}

vector<shared_ptr<PkbGraphNode>> PkbGraphNextStarRelationshipExtractor::extractAllThatReachEndDfs(shared_ptr<PkbGraphNode> startNode, shared_ptr<PkbGraphNode> endNode) {
	// 1. check if is target node. if yes, return self.
	if (startNode->equals(endNode)) {
		return vector<shared_ptr<PkbGraphNode>>{startNode};
	}

	// 2. check if there are any neighbours left 
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();
	vector<shared_ptr<PkbGraphNode>> unvisited;
	string key = startNode->getKey();
	for (shared_ptr<PkbGraphNode> n : neighbours) {
		if (!this->visitedEdges.count(pair<string, string>(key, n->getKey()))) {
			unvisited.push_back(n);
		}
	}

	// 2.1 if no neighbours left, terminal, but is not desired. return empty vector
	if (unvisited.size() == 0) {
		return vector<shared_ptr<PkbGraphNode>>();
	}

	// 2.2 else, do dfs on neighbours
	vector<shared_ptr<PkbGraphNode>> allDownstream;
	for (shared_ptr<PkbGraphNode> n : unvisited) {
		// get all downstream children
		vector<shared_ptr<PkbGraphNode>> downstream = this->extractAllThatReachEndDfs(n, endNode);

		// for each downstream child, form relationship with self and it
		for (shared_ptr<PkbGraphNode> c : downstream) {
			// create relationship and add
			this->extractedRelationships.push_back(shared_ptr<PkbRelationship>(new PkbNextStarRelationship(startNode->getAsEntity(), c->getAsEntity())));
		}

		// extend
		allDownstream.insert(allDownstream.end(), downstream.begin(), downstream.end());
	}

	// set state
	this->extracted = true;

	return allDownstream;
}