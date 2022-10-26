#pragma once

#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>

#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>
#include <memory>
#include <vector>
#include <unordered_set>
#include <pkb/PkbException.h>
#include <iostream>

using namespace std;

/*
	Handles extraction from a control flow graph of nextstar relationships. 
*/
class PkbGraphNextStarRelationshipExtractor  {
private: 
	bool extracted = false;
	vector<shared_ptr<PkbRelationship>> extractedRelationships; // object-level variable as method is recursive
	unordered_set<string> extractedRelationshipKeys; // keys to track what has been extracted
	unordered_set<string> cycleVisited; // set to track which nodes have been hit as a cycle before to avoid double cycles

	/*
		Cumulatively extracts all nextstar relationships from the specified start node by
		fully traversing the graph.
	*/
	vector<shared_ptr<PkbControlFlowGraphNode>> extractAllFromStartDfs(shared_ptr<PkbControlFlowGraphNode> startNode);

	/*
		Cumulatively extracts all nextstar relationships that end at the specified node, starting from the
		specified start node.
	*/
	vector<shared_ptr<PkbControlFlowGraphNode>> extractAllThatReachEndDfs(shared_ptr<PkbControlFlowGraphNode> startNode, shared_ptr<PkbControlFlowGraphNode> endNode);


public:

	/*
		Cumulatively extracts all nextstar relationships from the specified start node by 
		fully traversing the graph.
	*/
	void extractAllFromStart(shared_ptr<PkbControlFlowGraphNode> startNode, bool startOnlyFromStart = false) {

		this->extractAllFromStartDfs(startNode);

		// if start only from specified, then we filter
		vector<shared_ptr<PkbRelationship>> out;
		if (startOnlyFromStart) {
			for (shared_ptr<PkbRelationship> r : this->extractedRelationships) {
				if (r->getLhs()->equals(startNode->getAsEntity())) {
					out.push_back(r);
				}
			}
			this->extractedRelationships = out;
		}
		
	}

	/*
		Cumulatively extracts all nextstar relationships that end at the specified node, starting from the 
		specified start node.
	*/
	void extractAllThatReachEnd(shared_ptr<PkbControlFlowGraphNode> startNode, shared_ptr<PkbControlFlowGraphNode> endNode, bool endOnlyAtEnd = false) {
		this->extractAllThatReachEndDfs(startNode, endNode);

		// if start only from specified, then we filter
		vector<shared_ptr<PkbRelationship>> out;
		if (endOnlyAtEnd) {
			for (shared_ptr<PkbRelationship> r : this->extractedRelationships) {
				if (r->getRhs()->equals(endNode->getAsEntity())) {
					out.push_back(r);
				}
			}
			this->extractedRelationships = out;
		}
		
	}

	vector<shared_ptr<PkbRelationship>> getExtractedRelationships() {
		if (!this->extracted) {
			throw PkbException("Trying to get extracted NextStar, but not extracted yet!");
		}
		return this->extractedRelationships;
	}

};

