#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/design_extractor/Extractor.h>

#include <memory>
#include <unordered_set>

using namespace std;

class NextExtractor {

public:
	/*
		This method is used to extract relationships from a provided control flow graph.
	*/
	virtual vector<Relationship> extract(shared_ptr<CFGNode> cfg);
};
