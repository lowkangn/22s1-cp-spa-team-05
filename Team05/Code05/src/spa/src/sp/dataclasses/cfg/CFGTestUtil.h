#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/exceptions/CFGException.h>

#include<unordered_map>
#include<memory>

shared_ptr<CFGNode> createCFG(unordered_map<int, shared_ptr<CFGNode>> &cfgNodes, unordered_map<int, vector<int>> adjList, int rootIndex) {
	// Add all the nodes
	for (pair<int, vector<int>> i: adjList) {
		int nodeToAddTo = i.first;
		vector<int> childrenIndexes = i.second;
		for (int nodeToAdd: childrenIndexes) {
			cfgNodes[nodeToAddTo]->addChild(cfgNodes[nodeToAdd]);
		}
	}
	
	// Return root
	return cfgNodes[rootIndex];
}