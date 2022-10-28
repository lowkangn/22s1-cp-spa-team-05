#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/design_extractor/CFGExtractor.h>

#include <memory>

using namespace std;

class NextExtractor : public CFGExtractor<Relationship> {

public:
	/*
		This method is used to extract relationships from a provided control flow graph.
	*/
	vector<Relationship> extract(shared_ptr<CFGNode> cfg) override;
};
