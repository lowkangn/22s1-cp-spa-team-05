#include <pkb/pkbQueryManager/graph_extractors/PkbGraphNextStarRelationshipExtractor.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextStarRelationship.h>
#include <iostream>

vector<shared_ptr<PkbControlFlowGraphNode>> PkbGraphNextStarRelationshipExtractor::extractAllFromStartDfs(shared_ptr<PkbControlFlowGraphNode> startNode) {

	
	// 1. get neighbours
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();

	// 2. for each neighbour, we visit
	// if neighbour statement number increasing, is cycle. we include but don't recurse on it	
	vector<shared_ptr<PkbControlFlowGraphNode>> allDownstream = {startNode};
	for (shared_ptr<PkbGraphNode> n : neighbours) {

		// cast
		shared_ptr<PkbControlFlowGraphNode> castedNeighbour = static_pointer_cast<PkbControlFlowGraphNode>(n);
		if (castedNeighbour == NULL) {
			throw PkbException("Tried to cast pkb graph node to cfg node, but couldn't.");
		}

		// if cycle (decreasing statement number), we don't recurse, but do add to downstream
		vector<shared_ptr<PkbControlFlowGraphNode>> downstream;
		if (castedNeighbour->getStatementLineNumber() <= startNode->getStatementLineNumber()) { // is cycle
			// if not here before, give second chance
			if (this->cycleVisited.count(startNode->getKey())) {
				downstream = { castedNeighbour };
			}
			else {
				this->cycleVisited.insert(startNode->getKey());
				downstream = this->extractAllFromStartDfs(castedNeighbour); // get all downstream children
			}
			
		}
		else {
			// get all downstream children
			downstream = this->extractAllFromStartDfs(castedNeighbour);
		}

		// for each downstream child, form relationship with self and it
		for (shared_ptr<PkbGraphNode> c : downstream) {

			// create relationship and add
			shared_ptr<PkbRelationship> toAdd = shared_ptr<PkbRelationship>(new PkbNextStarRelationship(startNode->getAsEntity(), c->getAsEntity()));
			if (!this->extractedRelationshipKeys.count(toAdd->getKey())) { // not already found
				this->extractedRelationships.push_back(toAdd);
				this->extractedRelationshipKeys.insert(toAdd->getKey());
			}
			
		}
	
		// extend
		allDownstream.insert(allDownstream.end(), downstream.begin(), downstream.end());
	}

	// set state
	this->extracted = true;


	return allDownstream;
}

vector<shared_ptr<PkbControlFlowGraphNode>> PkbGraphNextStarRelationshipExtractor::extractAllThatReachEndDfs(shared_ptr<PkbControlFlowGraphNode> startNode, shared_ptr<PkbControlFlowGraphNode> endNode) {

	// set state
	this->extracted = true;

	// 0. check if is target node. if yes, return self.
	if (startNode->equals(endNode)) {
		return vector<shared_ptr<PkbControlFlowGraphNode>>{startNode};
	}

	// 1. get neighbours
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();
	// 2. for each neighbour, we visit
	vector<shared_ptr<PkbControlFlowGraphNode>> allDownstream = { startNode };
	for (shared_ptr<PkbGraphNode> neighbour : neighbours) {

		// cast
		shared_ptr<PkbControlFlowGraphNode> castedNeighbour = static_pointer_cast<PkbControlFlowGraphNode>(neighbour);
		if (castedNeighbour == NULL) {
			throw PkbException("Tried to cast pkb graph node to cfg node, but couldn't.");
		}

		// if cycle (decreasing statement number)
		vector<shared_ptr<PkbControlFlowGraphNode>> downstream;
		if (castedNeighbour->getStatementLineNumber() <= startNode->getStatementLineNumber()) { // is cycle
			// if not here before, give second chance
			if (this->cycleVisited.count(startNode->getKey())) {
				downstream = { castedNeighbour };
			}
			else {
				this->cycleVisited.insert(startNode->getKey());
				downstream = this->extractAllFromStartDfs(castedNeighbour); // get all downstream children
			}

		}
		else {
			// get all downstream children
			downstream = this->extractAllFromStartDfs(castedNeighbour);
		}

		// for each downstream child, form relationship with self and it
		for (shared_ptr<PkbGraphNode> child : downstream) {

			// create relationship and add
			shared_ptr<PkbRelationship> toAdd = shared_ptr<PkbRelationship>(new PkbNextStarRelationship(startNode->getAsEntity(), child->getAsEntity()));
			if (!this->extractedRelationshipKeys.count(toAdd->getKey())) { // not already found
				this->extractedRelationships.push_back(toAdd);
				this->extractedRelationshipKeys.insert(toAdd->getKey());
			}

		}
		// extend
		allDownstream.insert(allDownstream.end(), downstream.begin(), downstream.end());
	}

	return allDownstream;
}