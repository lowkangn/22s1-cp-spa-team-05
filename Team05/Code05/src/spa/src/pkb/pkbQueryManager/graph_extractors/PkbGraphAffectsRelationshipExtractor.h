#pragma once

#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>
#include <pkb/pkbRepository/PkbRepository.h>

#include <memory>

using namespace std;

class PkbGraphAffectsRelationshipExtractor {
public: 
	/*
		Traverses the graph from a start node to an end node and checks if an affects relationship
		holds. 
	*/
	bool hasAffectsRelationship(shared_ptr<PkbControlFlowGraphNode> startNode, shared_ptr<PkbControlFlowGraphNode> endNode, shared_ptr<PkbRepository> repository);
};