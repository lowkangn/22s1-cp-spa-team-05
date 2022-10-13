#include <pkb/graph_extractors/PkbGraphNextStarRelationshipExtractor.h>
#include <pkb/design_objects/relationships/PkbNextStarRelationship.h>
#include <iostream>

vector<shared_ptr<PkbGraphNode>> PkbGraphNextStarRelationshipExtractor::extractAllFromStartDfs(shared_ptr<PkbGraphNode> startNode) {
	
	// check if there are any neighbours left 
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();
	vector<shared_ptr<PkbGraphNode>> unvisited;
	string key = startNode->getKey();
	cout << "entering, " << key << endl;
	for (shared_ptr<PkbGraphNode> n : neighbours) {
		
		if (!this->visitedEdges.count(pair<string, string>(key, n->getKey()))) {
			
			unvisited.push_back(n);
		}
	}

	// if no neighbours left, return itself in vector
	vector<shared_ptr<PkbGraphNode>> allDownstream = { startNode };
	if (unvisited.size() == 0) {
		return allDownstream;
	}
	// if have, for each neighbour, add to visited list, get all child from vector 
	
	for (shared_ptr<PkbGraphNode> n : unvisited) {

		// add to visited set
		this->visitedEdges.insert(pair<string, string>(key, n->getKey()));

		// get all downstream children
		vector<shared_ptr<PkbGraphNode>> downstream = this->extractAllFromStartDfs(n);

		// for each downstream child, form relationship with self and it
		for (shared_ptr<PkbGraphNode> c : downstream) {
			// create relationship and add
			shared_ptr<PkbRelationship> toAdd = shared_ptr<PkbRelationship>(new PkbNextStarRelationship(startNode->getAsEntity(), c->getAsEntity()));
			if (!this->extractedRelationshipKeys.count(toAdd->getKey())) { // not already found
				cout << "here" << key << ", " << c->getAsEntity()->getKey() << endl;
				this->extractedRelationships.push_back(toAdd);
				this->extractedRelationshipKeys.insert(toAdd->getKey());
			}
			
		}
	
		// extend
		allDownstream.insert(allDownstream.end(), downstream.begin(), downstream.end());
	}

	// set state
	this->extracted = true;
	cout << "leaving, " << key << endl;

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