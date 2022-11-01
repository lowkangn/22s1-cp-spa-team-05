#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <stack>


using namespace std;

class ModifiesExtractor : public Extractor<Relationship> {

private:
	/*
		This is used for performing DP operations on Modifies Call relationships. It maps the procedure name
		to relationships extracted.

		Parser guarantees no duplicate naming of procedures from the root program node, so mapping should be unique

		1. We use this map to look ahead from the root program node and add the list of procedures names, mapping to Modifies(proc, v)
		which represent the procedure, and list of ModifiesRelationships extracted from it

		2. For a procedure call, we use this map to get the variables modified by the callee procedure

		3. If the called procedure has been extracted before, we can simply use the entries in this map (DP-style)

		4. Otherwise, the entry mapping procedure name and its extracted relationships will not be in this map.
		We then add this procedure and its extracted relationships to the map for future use (i.e. in procedure calls)
	*/
	unordered_map<string, vector<Relationship>> procedureNameToRelationshipMap;

	/*
		This keeps track of all procedures coming from root node 
	*/

	unordered_map<string, shared_ptr<ASTNode>> allProcedures;

	/*
		This is an adjacency list representing the calls between procedures
		Used for detecting cycles in call statement (AST parsing validation)
	*/
	unordered_map<shared_ptr<ASTNode>, vector<shared_ptr<ASTNode>>> callGraph;

	/*
		Used to toggle whether call cycles will be checked during extraction.
		Set to true by default, but can be set to false in the constructor to speed up testing if needed
	*/
	bool checkCallCycleFlag;

	// HELPER FUNCTIONS
	
	/*
		DP map relationships are in the form of procedure p : variable v
		This function takes variable v (from rhs) and create relationship call c : variable v
	*/

	vector<Relationship> extractCallRelationshipFromProcedure(shared_ptr<ASTNode> callNode, vector<Relationship> relationships) {
		assert(callNode->isCallNode());

		vector<Relationship> converted;
		Entity leftHandSide = callNode->extractEntity();

		for (int i = 0; i < relationships.size(); i++) {
			Relationship currRelation = relationships[i];
			Entity rightHandSide = currRelation.getRhs();
			assert(rightHandSide.getType() == EntityType::VARIABLE);

			Relationship toAdd = Relationship::createModifiesRelationship(leftHandSide, rightHandSide);
			converted.push_back(toAdd);
		}
		return converted;
	}

	/*
		Goes through each subtree rooted at procedure node to find call nodes inside each procedure

		Modified DFS - instead of starting from the root node, we start from each proc node and populate
		the callGraph based on which procedure calls another procedure
	*/
	void constructCallGraph(unordered_map<string, shared_ptr<ASTNode>> procedures) {
		stack<shared_ptr<ASTNode>> astStack;

		// Perform DFS at every subtree rooted in each procedure node
		for (unordered_map<string, shared_ptr<ASTNode>>::iterator it = procedures.begin(); it != procedures.end(); ++it) {
			shared_ptr<ASTNode> currProcedureNode = it->second;
			astStack.push(currProcedureNode);

			while (!astStack.empty()) {
				shared_ptr<ASTNode> currNode = astStack.top();
				astStack.pop();

				// if current node is a call node, create edge between caller procedure (currProcedureNode) and called procedure
				if (currNode->isCallNode()) {
					// cast to CallASTNode
					shared_ptr<CallASTNode> callNode = dynamic_pointer_cast<CallASTNode>(currNode);

					// get child procedure node of callNode, which is a procedure node and get its name
					shared_ptr<ASTNode> callNodeChild = callNode->getProcedureName();
					string procedureCalledName = callNodeChild->extractEntity().getString();

					// get the top-level procedure referenced by the procedure name found above
					shared_ptr<ASTNode> procedureCalledNode = procedures.at(procedureCalledName);

					// add called procedure to adjacency list
					callGraph.at(currProcedureNode).push_back(procedureCalledNode);
				}

				// Populate the AST node stack to continue DFS
				for (shared_ptr<ASTNode> child : currNode->getChildren()) {
					astStack.push(child);
				}
			}
		}
	}


	/*
		Checks for cycles in callGraph

		@param	src	- the current node in question
		@param	callGraph - adjacency list with procedure nodes as the vertex, call relationships as the vertex
		@param	visited - unordered_map of ASTNode to bool representing whether we already have visited a node
		@param	dfsVis - unordered_map of ASTNode to bool representing which nodes are being processed in a single DFS call 

		Idea: check if we already visited a node (which is also currently a neighbour of another node)
		If already visited, then we check whether we are processing its neighbours. If so, then we have 
		found a cycle. Because we haven't finished processing the node's neighbours but we found an edge 
		from the node to its neighbours
	*/
	bool checkCallCycleUtil(shared_ptr<ASTNode> currNode, unordered_map<shared_ptr<ASTNode>, vector<shared_ptr<ASTNode>>> callGraph, unordered_map<shared_ptr<ASTNode>, bool> visited, unordered_map<shared_ptr<ASTNode>, bool> dfsVis) {
		// Find currNode in vis and neighbourVis and set that node and its neighbours to being visited (true)
		visited[currNode] = 1;
		dfsVis[currNode] = 1;

		for (shared_ptr<ASTNode> neighbour : callGraph.at(currNode)) {
			if (!visited.at(neighbour)) {
				// Continue DFS traversal until we either find cycle or finish
				if (checkCallCycleUtil(neighbour, callGraph, visited, dfsVis)) return true;
			}
			// If we have visited the same node from the same DFS call again, there is a cycle
			else if (dfsVis.at(neighbour)) {
				return true;
			}
		}

		// This DFS call is done, we're not visiting this node anymore. Set to 0
		dfsVis[currNode] = 0;
		return false;
	}

	bool hasCallCycle(unordered_map<shared_ptr<ASTNode>, vector<shared_ptr<ASTNode>>> callGraph) {
		// Map of visited nodes in the whole traversal
		unordered_map<shared_ptr<ASTNode>, bool> visited;

		// Map of nodes visited during a single DFS call
		unordered_map<shared_ptr<ASTNode>, bool> dfsVis;

		// Initialise map of visited nodes
		for (unordered_map<shared_ptr<ASTNode>, vector<shared_ptr<ASTNode>>>::iterator it = callGraph.begin(); it != callGraph.end(); ++it) {
			shared_ptr<ASTNode> currNode = it->first;
			visited.emplace(currNode, 0);
			dfsVis.emplace(currNode, 0);
		}

		// Begin DFS and cycle-checking
		for (unordered_map<shared_ptr<ASTNode>, bool>::iterator it = visited.begin(); it != visited.end(); ++it) {
			// Check if curr vertex has been visited
			if (!it->second) {
				if (checkCallCycleUtil(it->first, callGraph, visited, dfsVis)) return true;
			}
		}
		return false;
	}

public:

	/*
		Constructor that sets call cycle checking to true by default
		Can set to false to speed up testing, but only for valid non-cyclical programs
	*/
	ModifiesExtractor(bool willCheckCallCycles = true) : checkCallCycleFlag(willCheckCallCycles) { }

	/*
		This method is used to extract relationships from a provided abstract syntax tree. It is meant to be
		overriden for each specific relationship type.
	*/
	vector<Relationship> extract(shared_ptr<ASTNode> ast) override;

	/*
		This method extracts a modifies relationship from an Procedure node
	*/
	vector<Relationship> handleProcedure(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an Assign node
	*/
	vector<Relationship> handleAssign(shared_ptr<ASTNode> ast);

	/*
	This method extracts a modifies relationship from an Read node
	*/
	vector<Relationship> handleRead(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an While node
	*/
	vector<Relationship> handleWhile(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an If node
	*/
	vector<Relationship> handleIf(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an call node
	*/
	vector<Relationship> handleCall(shared_ptr<ASTNode> ast);

	/*
		This method recursively traverses the node adding the Modifies relationship for procedures
	*/
	vector<Relationship> recursiveContainerExtract(Entity& LHS, shared_ptr<ASTNode> ast);
};
