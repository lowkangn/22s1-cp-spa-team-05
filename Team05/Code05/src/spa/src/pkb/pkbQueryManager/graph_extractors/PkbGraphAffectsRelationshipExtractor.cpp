#include "PkbGraphAffectsRelationshipExtractor.h"
#include <pkb/pkbQueryManager/PkbRelationshipQueryHelper.h>

#include <qps/query/clause/ClauseArgument.h>


#include <assert.h>
bool PkbGraphAffectsRelationshipExtractor::hasAffectsRelationship(shared_ptr<PkbControlFlowGraphNode> startNode, shared_ptr<PkbControlFlowGraphNode> endNode, shared_ptr<PkbRepository> repository, unordered_map<string, shared_ptr<PkbEntity>> candidateVariableKeyValueMap)
{
	// sanity check
	assert(startNode->isAssignStatementNode() && endNode->isAssignStatementNode());

	/*
		NOTE: in this implementation we assume that the sanity checks have passed, and that
		this function is only called then. To be precise, the preconditions are: 
		- end node is reachable from start node 
		- there is some variables start modifies that end uses
		- both start and end are assign nodes
	*/

	// 0. get neighbours 
	vector<shared_ptr<PkbGraphNode>> neighbours = startNode->getNeighbours();

	// 1. for each neighbour, we visit
	vector<shared_ptr<PkbControlFlowGraphNode>> allDownstream = { startNode };
	for (shared_ptr<PkbGraphNode> neighbour : neighbours) {

		// 2. cast
		shared_ptr<PkbControlFlowGraphNode> castedNeighbour = static_pointer_cast<PkbControlFlowGraphNode>(neighbour);
		if (castedNeighbour == nullptr) {
			throw PkbException("Tried to cast pkb graph node to cfg node, but couldn't.");
		}

		// 3a. if is target node, return true
		if (castedNeighbour->getAsEntity() == endNode->getAsEntity()) {
			return true;
		}
		else { // 3b. if not target, check and remove all candidates it modifies
			// early terminate if possible
			ClauseArgument nodeAsLhs = ClauseArgument::createLineNumberArg(to_string(castedNeighbour->getStatementLineNumber()));
			ClauseArgument variable = ClauseArgument::createVariableArg("v");
			PkbRelationshipQueryHelper helper;
			vector<shared_ptr<PkbRelationship>> lhsModifies = helper.retrieveRelationshipsFromTablesByTypeAndLhsRhs(PkbRelationshipType::MODIFIES, nodeAsLhs, variable, repository);
			unordered_map<string, shared_ptr<PkbRelationship>> lhsModifiesMap;
			for (shared_ptr<PkbRelationship> relationship : lhsModifies) {
				string key = relationship->getLhs()->getKey();
				if (candidateVariableKeyValueMap.find(key) != candidateVariableKeyValueMap.end()) {
					candidateVariableKeyValueMap.erase(key);
				}
				if (candidateVariableKeyValueMap.size() == 0) {
					return false; // early terminate on this path
				}
			}
		}

		// 4. cycle detection. if cycle, we give second chance
		if (castedNeighbour->getStatementLineNumber() <= startNode->getStatementLineNumber()) { // cycle
			// if been here for the second time, terminate
			if (this->cycleVisited.count(startNode->getKey())) {
				return false;
			}
			else { // else, mark, and continue
				this->cycleVisited.insert(startNode->getKey());
			}
			
		}

		// 5. recurse and return 
		return this->hasAffectsRelationship(castedNeighbour, endNode, repository, candidateVariableKeyValueMap);


	}
}
