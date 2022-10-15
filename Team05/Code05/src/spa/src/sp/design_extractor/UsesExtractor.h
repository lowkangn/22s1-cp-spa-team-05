#pragma once

#include <vector>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <memory>
#include <unordered_map>
#include <assert.h>

using namespace std;

class UsesExtractor : public Extractor<Relationship> {
private:
	/*
		This is used for performing DP operations on Modifies Call relationships. It maps the procedure name
		to relationships extracted.

		Parser guarantees no duplicate naming of procedures from the root program node, so mapping should be unique

		1. We use this map to look ahead from the root program node and add the list of procedures names, mapping to Uses(proc, v)
		which represent the procedure, and list of UsesRelationships extracted from it

		2. For a procedure call, we use this map to get the variables used by the callee procedure

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

			Relationship toAdd = Relationship::createUsesRelationship(leftHandSide, rightHandSide);
			converted.push_back(toAdd);
		}
		return converted;
	}

public:
	/*
		This method is used to extract relationships from a provided abstract syntax tree. It is meant to be
		overriden for each specific relationship type.
	*/
	vector<Relationship> extract(shared_ptr<ASTNode> ast) override;

	/*
		This method extracts a uses relationship from an Procedure node
	*/
	vector<Relationship> handleProcedure(shared_ptr<ASTNode> ast);

	/*
		This method extracts a uses relationship from an Assign node
	*/
	vector<Relationship> handleAssign(shared_ptr<ASTNode> ast);

	/*
	This method extracts a uses relationship from an Print node
	*/
	vector<Relationship> handlePrint(shared_ptr<ASTNode> ast);

	/*
		This method extracts a uses relationship from an While node
	*/
	vector<Relationship> handleWhile(shared_ptr<ASTNode> ast);

	/*
		This method extracts a uses relationship from an If node
	*/
	vector<Relationship> handleIf(shared_ptr<ASTNode> ast);

	/*
		This method extracts a uses relationship from an call node
	*/
	vector<Relationship> handleCall(shared_ptr<ASTNode> ast);

	/*
		This method extracts all the variables from an expression node
	*/
	vector<Entity> extractVariables(shared_ptr<ASTNode> ast);

	/*
		This method forms uses relationships between the given LHS entity and all the given variables
	*/
	void addRelationshipsWithVariables(vector<Relationship>& relationships, Entity& LHS, vector<Entity>& variables);

	/*
		This method recursively traverses the node adding the uses relationship for procedures
	*/
	vector<Relationship> recursiveContainerExtract(Entity& LHS, shared_ptr<ASTNode> ast);
};
