#include <pkb/graph_extractors/PkbGraphNextStarRelationshipExtractor.h>
#include <pkb/design_objects/relationships/PkbNextStarRelationship.h>
#include <iostream>

vector<shared_ptr<PkbControlFlowGraphNode>> PkbGraphNextStarRelationshipExtractor::extractAllFromStartDfs(shared_ptr<PkbControlFlowGraphNode> startNode) {

	// 0. add to visiting
	this->visitingNodes.insert(startNode->getKey());

	// 1. get neighbours
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();
	cout << "\nentering " << startNode->getAsEntity()->getKey() << " with " << neighbours.size() << " neighbours." << endl;
	// 2. for each neighbour, we visit
	// if neighbour statement number increasing, is cycle. we include but don't recurse on it	
	vector<shared_ptr<PkbControlFlowGraphNode>> allDownstream = {startNode};
	for (shared_ptr<PkbGraphNode> n : neighbours) {

		// cast
		shared_ptr<PkbControlFlowGraphNode> casted_n = static_pointer_cast<PkbControlFlowGraphNode>(n);
		if (n == NULL) {
			throw PkbException("Tried to cast pkb graph node to cfg node, but couldn't.");
		}

		// if cycle (decreasing statement number), we don't recurse, but do add to downstream
		vector<shared_ptr<PkbControlFlowGraphNode>> downstream;
		if (this->visitingNodes.count(casted_n->getKey())) {
			cout << "Cycle found" << endl;
			downstream = { casted_n };
		}
		else {
			// get all downstream children
			downstream = this->extractAllFromStartDfs(casted_n);
			cout << "here instead" << downstream.size() << endl;
		}

		// for each downstream child, form relationship with self and it
		for (shared_ptr<PkbGraphNode> c : downstream) {

			// create relationship and add
			shared_ptr<PkbRelationship> toAdd = shared_ptr<PkbRelationship>(new PkbNextStarRelationship(startNode->getAsEntity(), c->getAsEntity()));
			cout << "Considering new relationship   " << startNode->getAsEntity()->getKey() << ", " << c->getAsEntity()->getKey() << endl;
			if (!this->extractedRelationshipKeys.count(toAdd->getKey())) { // not already found
				cout << "New relationship, adding  " << startNode->getAsEntity()->getKey() << ", " << c->getAsEntity()->getKey() << endl;
				this->extractedRelationships.push_back(toAdd);
				this->extractedRelationshipKeys.insert(toAdd->getKey());
			}
			
		}
	
		// extend
		allDownstream.insert(allDownstream.end(), downstream.begin(), downstream.end());
		cout << "leaving " << startNode->getAsEntity()->getKey() << endl;
	}

	// set state
	this->extracted = true;

	// as you return, remove from visiting
	this->visitingNodes.erase(startNode->getKey());

	return allDownstream;
}

vector<shared_ptr<PkbControlFlowGraphNode>> PkbGraphNextStarRelationshipExtractor::extractAllThatReachEndDfs(shared_ptr<PkbControlFlowGraphNode> startNode, shared_ptr<PkbControlFlowGraphNode> endNode) {
	// 1. check if is target node. if yes, return self.
	if (startNode->equals(endNode)) {
		return vector<shared_ptr<PkbControlFlowGraphNode>>{startNode};
	}

	// 2. get neighbours
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();

	// 3. for each neighbour, do
	// if neighbour statement number increasing, is cycle. we include but don't recurse on it	
	vector<shared_ptr<PkbControlFlowGraphNode>> allDownstream;
	bool hasOneNonCycleChild = false;
	for (shared_ptr<PkbGraphNode> n : neighbours) {

		// cast
		shared_ptr<PkbControlFlowGraphNode> casted_n = static_pointer_cast<PkbControlFlowGraphNode>(n);
		if (n == NULL) {
			throw PkbException("Tried to cast pkb graph node to cfg node, but couldn't.");
		}

		// if cycle (decreasing statement number), we don't recurse, but do add to downstream
		vector<shared_ptr<PkbControlFlowGraphNode>> downstream;
		if (casted_n->getStatementLineNumber() < startNode->getStatementLineNumber()) {
			downstream = { casted_n };
		}
		else {
			// get all downstream children
			hasOneNonCycleChild = true;
			downstream = this->extractAllFromStartDfs(casted_n);
		}

		// for each downstream child, form relationship with self and it
		for (shared_ptr<PkbGraphNode> c : downstream) {

			// create relationship and add
			shared_ptr<PkbRelationship> toAdd = shared_ptr<PkbRelationship>(new PkbNextStarRelationship(startNode->getAsEntity(), c->getAsEntity()));
			if (!this->extractedRelationshipKeys.count(toAdd->getKey())) { // not already found
				cout << "adding" << startNode->getAsEntity()->getKey() << ", " << c->getAsEntity()->getKey() << endl;
				this->extractedRelationships.push_back(toAdd);
				this->extractedRelationshipKeys.insert(toAdd->getKey());
			}
		}

		// if no neighbours, failed to find. same if terminal node in the sense that is just a cycle, failed to find target
		if (neighbours.size() == 0 || !hasOneNonCycleChild) {
			return vector<shared_ptr<PkbControlFlowGraphNode>>{};
		}

		// extend
		allDownstream.insert(allDownstream.end(), downstream.begin(), downstream.end());

		
	}
	return allDownstream;
}