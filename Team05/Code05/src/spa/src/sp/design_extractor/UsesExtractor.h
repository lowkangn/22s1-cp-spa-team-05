#pragma once

#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <memory>

using namespace std;

class UsesExtractor : public Extractor<Relationship> {

public:
	/*
		This method is used to extract entities from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
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
