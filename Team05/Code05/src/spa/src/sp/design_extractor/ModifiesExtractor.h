#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <memory>
#include <unordered_map>

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

	vector<shared_ptr<ASTNode>> allProcedures;

public:
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
